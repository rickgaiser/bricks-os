#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/elf.h"
#include "kernel/memoryManager.h"
#include "kernel/settings.h"
#include "kernel/syscall.h"
#include "kernel/task.h"

#include "asm/arch/config.h"
#include "asm/irq.h"
#include "asm/cpu.h"

#include "apic.h"
#include "cpuid.h"
#include "keyboard.h"
#include "descriptor.h"
#include "mmap.h"
#include "multiboot.h"
#include "task.h"
#include "serial.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // #ifdef CONFIG_DEBUGGING

#ifdef CONFIG_FILESYSTEM
#include "kernel/fileSystem.h"
#include "kernel/ibmPartitionDriver.h"
#include "kernel/fatDriver.h"
#include "ata.h"
#endif // #ifdef CONFIG_FILESYSTEM

#include "string.h"


extern char       start_text;
extern char       end_bss;

CIRQ              cIRQ;
CI386Keyboard     cKeyboard;
//CPCTask           taskTest;
CI386Serial       cSerial;

#ifdef CONFIG_DEBUGGING
CI386DebugScreen  cDebug;
#endif // #ifdef CONFIG_DEBUGGING

bool              bPAEEnabled;
bool              bAPICEnabled;

SSetting settings[] = {
    {"PAE",  SET_AUTO}
  , {"APIC", SET_AUTO}
};
CSettings cSettings(settings, sizeof(settings) / sizeof(SSetting));
/*
// -----------------------------------------------------------------------------
int
loadELF32(void * file, CPCTask & task)
{
  static const char magic[] = {0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  Elf32_Ehdr * hdr = (Elf32_Ehdr *)file;

  // Check header magic
  for(int i(0); i < 16; i++)
    if(magic[i] != hdr->e_ident[i])
      return -1;

  // Load Sections
  for(int i(0); i < hdr->e_phnum; i++)
  {
    Elf32_Phdr * seg = (Elf32_Phdr *)((char *)file + hdr->e_phoff + hdr->e_phentsize * i);
    switch(seg->p_type)
    {
      case SHT_PROGBITS:
        printk(" - vaddr: %d, foff: %d, fsize: %d, msize: %d\n", seg->p_vaddr, seg->p_offset, seg->p_filesz, seg->p_memsz);
        task.aspace().addSection((char *)seg->p_vaddr, (char *)file + seg->p_offset, seg->p_filesz);
        break;
      default:
        printk("Unable to load segment type: %d\n", seg->p_type);
        return -1;
    };
  }

  // Set task entry point
  task.entry((void *)hdr->e_entry);
  task.state(TS_READY);

  return 0;
}
*/
// -----------------------------------------------------------------------------
int
main(unsigned long magic, multiboot_info_t * mbi)
{
  int iRetVal(0);
  unsigned char * pFirstFreeByte = 0;
  uint64_t iMemFree(0);
  uint64_t iMemTop(0);
  uint64_t iMemReserved(0);
  uint64_t iMemKernel(0);
  unsigned int iMemPageCount(0);

#ifdef CONFIG_DEBUGGING
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // #ifdef CONFIG_DEBUGGING

  if(cIRQ.init() == -1)
    iRetVal = -1;
  if(cKeyboard.init() == -1)
    iRetVal = -1;
  if(cSerial.init() == -1)
    iRetVal = -1;
  cSerial.cCom1_.write("Bricks-OS\n", 10);
  cSerial.cCom1_.write("=========\n", 10);

  // ---------------------------------------
  // Miltiboot loader and memorymap required
  if(magic == MULTIBOOT_BOOTLOADER_MAGIC)
  {
    if((mbi->flags & (1<<6)) == 0)
    {
      panic("ERROR: No multiboot memory map present\n");
    }
  }
  else
  {
    panic("ERROR: Multiboot loader information not present\n");
  }

  // -----------------------------
  // Initialize CPU identification
  //CPU::init();

  // --------------------------------
  // Setup Physical Memory Management
  // --------------------------------
  // Memory Map Type:
  // 0x01 memory, available to OS
  // 0x02 reserved, not available (e.g. system ROM, memory-mapped device)
  // 0x03 ACPI Reclaim Memory (usable by OS after reading ACPI tables) (60 KiB ?)
  // 0x04 ACPI NVS Memory (OS is required to save this memory between NVS sessions) (4 KiB ?)
  //printk("Memory Map:\n");
  for(memory_map_t * mmap = (memory_map_t *) mbi->mmap_addr; (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length; mmap = (memory_map_t *) ((unsigned long)mmap + mmap->size + sizeof (mmap->size)))
  {
    //printk(" - Type: %d, Addr: %d, Size: %d\n", mmap->type, mmap->base_addr_low, mmap->length_low);
    // Determine top of memory
    // Determine free memory
    if(mmap->type == 1)
    {
      if(iMemTop < (mmap->base_addr_low + mmap->length_low))
        iMemTop = mmap->base_addr_low + mmap->length_low;
      iMemFree += mmap->length_low;
    }
  }

  if(iMemFree < (2 * 1024 * 1024))
  {
    panic("ERROR: %dKiB free memory, need at least 2048KiB\n", iMemFree/1024);
  }

  iMemReserved = iMemTop - iMemFree;
  iMemPageCount = iMemTop / 4096;

  // Kernel location and size
  //printk("Kernel: start: %d, size: %d\n", (unsigned int)&start_text, (unsigned int)&end_bss - (unsigned int)&start_text);
  if((unsigned char *)&end_bss > pFirstFreeByte)
    pFirstFreeByte = (unsigned char *)&end_bss;
  // Module location and size
  if((mbi->flags & (1<<3)) && ((int)mbi->mods_count > 0))
  {
    module_t * mod = (module_t *) mbi->mods_addr;
    for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
    {
      //printk("Module: start: %d, size: %d\n", mod->mod_start, mod->mod_end - mod->mod_start);
      if((unsigned char *)mod->mod_end > pFirstFreeByte)
        pFirstFreeByte = (unsigned char *)mod->mod_end;
    }
  }

  // Create physical memory map, all pages will be initially marked used
  uint8_t * pMemoryMap = pFirstFreeByte;
  init_mmap(pMemoryMap, iMemPageCount);
  //printk("MemMap: start: %d, size: %d\n", (unsigned int)pFirstFreeByte, iMemPageCount);
  pFirstFreeByte += iMemPageCount;

  // Free memory that multiboot/bios reports available
  for(memory_map_t * mmap = (memory_map_t *) mbi->mmap_addr; (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length; mmap = (memory_map_t *) ((unsigned long)mmap + mmap->size + sizeof (mmap->size)))
    if(mmap->type == 1)
      physFreeRange(mmap->base_addr_low, mmap->length_low);

  // Allocate interrupt area (start: 0, size: 4KiB)
  physAllocRange((uint64_t)0x00000000, 0x00001000);
  // Allocate kernel stack (start: 1MiB, size: 4KiB)
  physAllocRange((uint64_t)0x00100000, 0x00001000);
  // Allocate kernel
  physAllocRange((uint64_t)&start_text, (unsigned int)&end_bss - (unsigned int)&start_text);
  // Allocate modules
  if((mbi->flags & (1<<3)) && ((int)mbi->mods_count > 0))
  {
    module_t * mod = (module_t *) mbi->mods_addr;
    for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
      physAllocRange((uint64_t)mod->mod_start, mod->mod_end - mod->mod_start);
  }
  // Allocate memory map
  physAllocRange((uint64_t)pMemoryMap, iMemPageCount);

  // ----------------
  // Setup Interrupts
  // ----------------
  // Allocate & Create IDT (Max.: 8-Byte *  256 =  2-KiB)
  physAllocRange((uint64_t)pFirstFreeByte, sizeof(SDescriptor) *  256);
  //printk("IDT   : start: %d, size: %d\n", (unsigned int)pFirstFreeByte, sizeof(SDescriptor) * 256);
  init_idt((SDescriptor *)pFirstFreeByte,  256);
  pFirstFreeByte += sizeof(SDescriptor) *  256;

  // ---------
  // Setup GDT
  // ---------
  // Align to page boundries (4KiB)
  pFirstFreeByte = (unsigned char *)PAGE_ALIGN_UP_4K(pFirstFreeByte);
  // Allocate & Create GDT (Max.: 8-Byte * 8192 = 64-KiB)
  physAllocRange((uint64_t)pFirstFreeByte, sizeof(SDescriptor) * 8192);
  //printk("GDT   : start: %d, size: %d\n", (unsigned int)pFirstFreeByte, sizeof(SDescriptor) * 8192);
  init_gdt((SDescriptor *)pFirstFreeByte, 8192);
  pFirstFreeByte += sizeof(SDescriptor) * 8192;

  // All static memory has been allocated now. At this point we create our heap for dynamic memory
  // After this we can use new/delete/malloc/free
  physAllocRange((uint64_t)pFirstFreeByte, 1 * 1024 * 1024);
  init_heap(pFirstFreeByte, 1 * 1024 * 1024);

  // ------------------------------------
  // Parse settings from the command line
  if(mbi->flags & (1<<2))
  {
    cSettings.parse((char *)mbi->cmdline);
  }

  // --------
  // Use PAE?
  if(CPU::hasPAE())
  {
    // PAE can be enabled/disabled on the command line (if present)
    switch(cSettings.get("PAE"))
    {
      case SET_AUTO:
        // PAE will only be enabled if physical memory is located above the 4GiB limit
        // Note: Do we need PAE for page level data execution prevention?
        //       Why not use segment level data execution prevention?
        if(iMemTop > (uint64_t(4 * 1024 * 1024) * 1024))
          bPAEEnabled = true;
        else
          bPAEEnabled = false;
        break;
      case SET_ON:
        bPAEEnabled = true;
        break;
      case SET_OFF:
        bPAEEnabled = false;
        break;
    };

    // Display status
    if(bPAEEnabled == true)
      printk("PAE enabled\n");
    else
      printk("PAE disabled\n");
  }
  else
  {
    bPAEEnabled = false;
  }

  // ---------
  // Use APIC?
  if(CPU::hasAPIC())
  {
    // APIC can be enabled/disabled on the command line (if present)
    switch(cSettings.get("APIC"))
    {
      case SET_AUTO:
      case SET_ON:
        bAPICEnabled = true;
        break;
      case SET_OFF:
        bAPICEnabled = false;
        break;
    };

    // Display status
    if(bAPICEnabled == true)
      printk("APIC enabled\n");
    else
      printk("APIC disabled\n");
  }
  else
  {
    bAPICEnabled = false;
  }

  //if(bAPICEnabled == true)
  //  init_apic();

  // ---------------------
  // Setup Paging
  // Setup Task Management
  // ---------------------
  CPCThread::init();
  printk("Paging Enabled!\n");
/*
  // ------------
  // Load modules
  // ------------
  if((mbi->flags & (1<<3)) && ((int)mbi->mods_count > 0))
  {
    //printk("Found Modules: %d\n", (int)mbi->mods_count);
    module_t * mod = (module_t *) mbi->mods_addr;
    for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
    {
      printk("Loading: %s\n", (char *)mod->string);

      // Try to load as elf32 file
      taskTest.init();
      taskTest.aspace().addRange(taskMain.aspace(), 0, 0x00400000);  // Bottom 4MiB
      if(loadELF32((void *)mod->mod_start, taskTest) == 0)
        printk(" - Done\n");
      else
        printk(" - Unknown File\n");
    }
  }
*/

  // Enable Timer IRQ
  cIRQ.enable(0x20);

#ifdef CONFIG_FILESYSTEM
  CIBMPartitionDriver ibmPartitionDriver;
  CFATDriver fatDriver;
  CATADriver ataPriMaster(0x1f0, true);
  //CATADriver ataPriSlave (0x1f0, false);
  //CATADriver ataSecMaster(0x170, true);
  //CATADriver ataSecSlave (0x170, false);

  CFileSystem::addPartitionDriver(&ibmPartitionDriver);
  CFileSystem::addFileSystemDriver(&fatDriver);
  ataPriMaster.init();
  //ataPriSlave.init();
  //ataSecMaster.init();
  //ataSecSlave.init();
#endif // CONFIG_FILESYSTEM

  iMemKernel = iMemTop - iMemReserved - (freePageCount() * 4096);
  printk("Memory size:     %dKiB\n", iMemTop/1024);
  printk("Memory reserved: %dKiB\n", iMemReserved/1024);
  printk("Memory kernel:   %dKiB\n", iMemKernel/1024);
  printk("Memory free:     %dKiB\n", freePageCount() * 4);

  // Enable interrupts
  local_irq_enable();
  printk("Interrupts...OK\n");

  printk("PC arch ready\n");

  {
    CV86Thread v86thr;
/*
    const char * v86_msg = "Hello from V86 mode";
    char * s = (char *)physAllocPageLow();
    strcpy(s, v86_msg);

    printk("Demo 1: int 0x10 ah=0x0e (display text character)\n");
    printk("s: %s -> %s\n", v86_msg, s);
    for(; *s != 0; s++)
    {
      v86thr.pTSS_->eax = 0x0e00 | *(unsigned char *)s;
      v86thr.pTSS_->ebx = 0x0000;
      v86thr.interrupt(0x10);
    }
    printk("\nDemo 1: Done\n");
*/
    printk("Demo 3: int 0x10 ah=0x00 (VGA mode-set; 80x50 text)\n");
    // set 80x25 text mode
    v86thr.pTSS_->eax = 0x0003;
    v86thr.interrupt(0x10);
    // set 8x8 font for 80x50 text mode
    v86thr.pTSS_->eax = 0x1112;
    v86thr.pTSS_->ebx = 0;
    v86thr.interrupt(0x10);
    printk("Demo 3: Done\n");
  }

  return bricks_main();
}
