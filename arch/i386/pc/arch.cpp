#include "kernel/bricks.h"
#include "kernel/task.h"
#include "kernel/elf.h"
#include "kernel/syscall.h"
#include "kernel/srr.h"
#include "asm/irq.h"
#include "asm/cpu.h"
#include "video.h"
#include "keyboard.h"
#include "descriptor.h"
#include "multiboot.h"
#include "string.h"
#include "iostream"


extern char       start_text;
extern char       end_bss;
CIRQ              cIRQ;
CI386Video        cVideo;
CI386Keyboard     cKeyboard;

STaskStateSegment taskMain;
STaskStateSegment taskTest;
selector_t        selTaskMain;
selector_t        selTaskTest;
uint8_t           stack_pl0[512];
uint8_t           stack_pl3[512];


#define PAGE_ALIGN_DOWN_4K(addr) (addr & 0xfffff000)
#define PAGE_ALIGN_DOWN_2M(addr) (addr & 0xffe00000)
#define PAGE_ALIGN_DOWN_4M(addr) (addr & 0xffc00000)

#define PAGE_ALIGN_UP_4K(addr)   ((addr & 0x00000fff) ? (PAGE_ALIGN_DOWN_4K(addr) + 0x00001000) : (addr))
#define PAGE_ALIGN_UP_2M(addr)   ((addr & 0x001fffff) ? (PAGE_ALIGN_DOWN_2M(addr) + 0x00200000) : (addr))
#define PAGE_ALIGN_UP_4M(addr)   ((addr & 0x003fffff) ? (PAGE_ALIGN_DOWN_4M(addr) + 0x00400000) : (addr))


unsigned int    iMemSize(0);
unsigned int    iMemPageCount(0);
unsigned char * pMemMap(0);
// -----------------------------------------------------------------------------
bool
allocRange(void * start, unsigned int length)
{
  unsigned int index = PAGE_ALIGN_DOWN_4K((unsigned int)start) >> 12;
  unsigned int end = PAGE_ALIGN_UP_4K((unsigned int)start + length) >> 12;
  
  // Validate range
  if(end > iMemPageCount)
    return false;

  // Allocate
  while(index < end)
    pMemMap[index++] = 1;
  
  return true;
}

// -----------------------------------------------------------------------------
bool
freeRange(void * start, unsigned int length)
{
  unsigned int index = PAGE_ALIGN_DOWN_4K((unsigned int)start);
  unsigned int end = PAGE_ALIGN_UP_4K((unsigned int)start + length);
  
  // Validate range
  if(end > iMemPageCount)
    return false;

  // Free
  while(index < end)
    pMemMap[index++] = 0;

  return true;
}

// -----------------------------------------------------------------------------
void
ttt()
{
  while(true)
  {
    std::cout<<"Hello, I am a ring3 task"<<std::endl;
    
    // Test system call
    //char recvBuffer[20];
    //msgReceive(scTEST, recvBuffer, 20);
    //std::cout<<"server: "<<recvBuffer<<std::endl;
    //msgReply(scTEST, 0, "SRV", 4);
    
    // Return to kernel
    jumpSelector(selTaskMain);
  }
}

// -----------------------------------------------------------------------------
void
testTask(void * entry)
{
  // Setup test task
  memset(&taskTest, 0, sizeof(STaskStateSegment));
  taskTest.esp0 = (uint32_t)&stack_pl0 + 512;
  taskTest.ss0  = selDataSys;
  taskTest.esp  = (uint32_t)&stack_pl3 + 512;
  taskTest.eip  = (uint32_t)entry;
  taskTest.eflags = 0x200;
  taskTest.es   = selDataUser;
  taskTest.cs   = selCodeUser;
  taskTest.ss   = selDataUser;
  taskTest.ds   = selDataUser;
  taskTest.fs   = selDataUser;
  taskTest.gs   = selDataUser;
  
  selTaskMain = cGDT.createSegment(dtTSS, 3, (uint32_t)&taskMain, sizeof(STaskStateSegment));
  selTaskTest = cGDT.createSegment(dtTSS, 0, (uint32_t)&taskTest, sizeof(STaskStateSegment));

  // Set the current running tasks tss
  setTR(selTaskMain);
  
  // Jump to test task
  std::cout<<"Starting test task"<<std::endl;
  jumpSelector(selTaskTest);
  std::cout<<"Test task returned"<<std::endl;
}

// -----------------------------------------------------------------------------
int
loadELF32(void * file)
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
        //std::cout<<"  - Segment off: "<<seg->p_offset
        //         <<", vaddr: "<<seg->p_vaddr
        //         <<", paddr: "<<seg->p_paddr
        //         <<", fsize: "<<seg->p_filesz
        //         <<", msize: "<<seg->p_memsz<<std::endl;
        memcpy((char *)seg->p_vaddr, (char *)file + seg->p_offset, seg->p_filesz);
        break;
      default:
        std::cout<<"Unable to load segment type: "<<seg->p_type<<std::endl;
        return -1;
    };    
  }
  
  // Jump to entry point
  //std::cout<<"  - Entry point: "<<hdr->e_entry<<std::endl;
  typedef void (*fptr)();
  fptr entry = (fptr)hdr->e_entry;
  entry();
  
  return 0;
}

// -----------------------------------------------------------------------------
int
loadELF64(void * file)
{
  static const char magic[] = {0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  Elf64_Ehdr * hdr = (Elf64_Ehdr *)file;
  
  // Check header magic
  for(int i(0); i < 16; i++)
    if(magic[i] != hdr->e_ident[i])
      return -1;
  
  return 0;
}

// -----------------------------------------------------------------------------
int
main(unsigned long magic, multiboot_info_t * mbi)
{
  int iRetVal(0);
  unsigned char * pFirstFreeByte(0);

  if(cIRQ.init() == -1)
    iRetVal = -1;
  if(cVideo.init() == -1)
    iRetVal = -1;
  if(cKeyboard.init() == -1)
    iRetVal = -1;

  CTaskManager::setStandardOutput(&cVideo);
  CTaskManager::setStandardInput(&cKeyboard);

  // Determine total memory size
  // Use multiboot Memory Map (if present)
  if((magic == MULTIBOOT_BOOTLOADER_MAGIC) && (mbi->flags & (1<<6)))
  {
    // Using multiboot Memory Map
    for(memory_map_t * mmap = (memory_map_t *) mbi->mmap_addr; (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length; mmap = (memory_map_t *) ((unsigned long)mmap + mmap->size + sizeof (mmap->size)))
      if((mmap->type == 1) || (mmap->type == 3) || (mmap->type == 4))
        if(iMemSize < mmap->base_addr_low + mmap->length_low)
          iMemSize = mmap->base_addr_low + mmap->length_low;
  }
  else
  {
    // Using Dirty Detector
    // Note: memory is assumed to be in multiples of at least 1MiB
    volatile unsigned int * pMem = (unsigned int *)((4*1024*1024) - sizeof(unsigned int));  // Start at 4MiB
    while(true)
    {
      unsigned int tmp = *pMem;
      *pMem = ~*pMem;
      if(*pMem != ~tmp)
        break;
      *pMem = tmp;
      pMem += 1024 * 1024;
    }
    pMem -= 1024;
    pMem += sizeof(unsigned int);
    iMemSize = (unsigned int)pMem;
  }
  std::cout<<"Memory size: "<<iMemSize/(1024*1024)<<"MiB"<<std::endl;
  iMemPageCount = iMemSize / 4096;
  
  // Kernel location and size
  std::cout<<"Kernel: start: "<<(unsigned int)&start_text<<", size:"<<(unsigned int)&end_bss - (unsigned int)&start_text<<std::endl;
  if((unsigned char *)&start_text > pFirstFreeByte)
    pFirstFreeByte = (unsigned char *)&start_text;
  
  // Module location and size
  if((magic == MULTIBOOT_BOOTLOADER_MAGIC) && (mbi->flags & (1<<3)) && ((int)mbi->mods_count > 0))
  {
    module_t * mod = (module_t *) mbi->mods_addr;
    for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
    {
      std::cout<<"Module: start: "<<mod->mod_start<<", size:"<<mod->mod_end - mod->mod_start<<std::endl;
      if((unsigned char *)mod->mod_end > pFirstFreeByte)
        pFirstFreeByte = (unsigned char *)mod->mod_end;
    }
  }
  
  // Create physical memory map
  pMemMap = pFirstFreeByte;
  memset(pMemMap, 0, iMemPageCount);
  std::cout<<"MemMap: start: "<<(unsigned int)pMemMap<<", size:"<<iMemPageCount<<std::endl;
  pFirstFreeByte += iMemPageCount;
  
  // Allocate first 1MiB of legacy memory
  allocRange((void *)0x00000000, 0x00100000);
  // Allocate kernel
  allocRange((void *)&start_text, (unsigned int)&end_bss - (unsigned int)&start_text);
  // Allocate modules
  if((magic == MULTIBOOT_BOOTLOADER_MAGIC) && (mbi->flags & (1<<3)) && ((int)mbi->mods_count > 0))
  {
    module_t * mod = (module_t *) mbi->mods_addr;
    for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
      allocRange((void *)mod->mod_start, mod->mod_end - mod->mod_start);
  }

  // Create IDT (Max.: 8-Byte *  256 =  2-KiB)
  allocRange((void *)pFirstFreeByte, sizeof(SDescriptor) *  256);
  init_idt((SDescriptor *)pFirstFreeByte,  256);
  pFirstFreeByte += sizeof(SDescriptor) *  256;

  // Create GDT (Max.: 8-Byte * 8192 = 64-KiB)
  allocRange((void *)pFirstFreeByte, sizeof(SDescriptor) * 8192);
  init_gdt((SDescriptor *)pFirstFreeByte, 8192);
  pFirstFreeByte += sizeof(SDescriptor) * 8192;
  
  /*
  // Try to use multiboot information
  if(magic == MULTIBOOT_BOOTLOADER_MAGIC)
  {
    // Multiboot loader detected
    std::cout<<"Multiboot loader detected"<<std::endl;
    
    // Memory
    if(mbi->flags & (1<<0))
    {
      std::cout<<"Lower Memory: "<<(unsigned int)mbi->mem_lower<<"KiB"<<std::endl;
      std::cout<<"Upper Memory: "<<(unsigned int)mbi->mem_upper/1024<<"MiB"<<std::endl;
    }
    
    // Boot device
    if(mbi->flags & (1<<1))
    {
      //printf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);
    }
    
    // Command line
    if(mbi->flags & (1<<2))
    {
      std::cout<<"Command line: "<<(char *)mbi->cmdline<<std::endl;
    }

    // a.out symbol table
    if(mbi->flags & (1<<4))
    {
      aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);
      
      printf ("aout_symbol_table: tabsize = 0x%0x, "
	      "strsize = 0x%x, addr = 0x%x\n",
	      (unsigned) aout_sym->tabsize,
	      (unsigned) aout_sym->strsize,
	      (unsigned) aout_sym->addr);
    }

    // ELF section header table
    if(mbi->flags & (1<<5))
    {
      elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

      printf ("elf_sec: num = %u, size = 0x%x,"
	      " addr = 0x%x, shndx = 0x%x\n",
	      (unsigned) elf_sec->num, (unsigned) elf_sec->size,
	      (unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
    }
  }
  */
  
  // Test Ring3 task
  testTask((void *)ttt);
  
  // Try to load multiboot modules
  if((magic == MULTIBOOT_BOOTLOADER_MAGIC) && (mbi->flags & (1<<3)) && ((int)mbi->mods_count > 0))
  {
    //std::cout<<"Found Modules: "<<(int)mbi->mods_count<<std::endl;
    module_t * mod = (module_t *) mbi->mods_addr;
    for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
    {
      std::cout<<" - Loading: "<<(char *)mod->string<<std::endl;
        
      // Try to load as elf32 file
      if(loadELF32((void *)mod->mod_start) != 0)
      {
        // Try to load as elf64 file
        if(loadELF64((void *)mod->mod_start) != 0)
        {
          std::cout<<"  - Unknown File";
        }
      }
    }
  }

  // Enable Timer IRQ
  //cIRQ.enable(0x20);

  return bricks_main();
}

