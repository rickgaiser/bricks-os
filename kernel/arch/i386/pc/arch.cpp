/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/elf.h"
#include "kernel/memoryManager.h"
#include "kernel/settings.h"
#include "kernel/syscall.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"

#include "asm/arch/config.h"
#include "asm/cpu.h"
#include "asm/task.h"

#include "apic.h"
#include "cpuid.h"
#include "descriptor.h"
#include "drivers.h"
#include "gpf.h"
#include "i8254.h"
#include "i8259.h"
#include "pmm.h"
#include "asm/vmm.h"
#include "multiboot.h"
#include "pci.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // #ifdef CONFIG_DEBUGGING

#include "string.h"


extern char       start_text;
extern char       end_bss;

CIRQ              c8259;       // Legacy IRQ, represents both master and slave controller
CI8254            c8254(0x40); // Legacy Timer

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


// -----------------------------------------------------------------------------
int
loadELF32(void * file, CTask & task)
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
//        task.aspace().addSection((char *)seg->p_vaddr, (char *)file + seg->p_offset, seg->p_filesz);
        break;
      default:
        printk(" - Unable to load segment type: %d\n", seg->p_type);
        return -1;
    };
  }

  // Set task entry point
//  task.entry((void *)hdr->e_entry);
//  task.state(TS_READY);

  return 0;
}

// -----------------------------------------------------------------------------
void
init_memmap(multiboot_info_t * mbi)
{
  uint64_t iMemFree = 0;
  uint64_t iMemTop = 0;
  
  // Determine usable memory
  if(mbi->flags & (1<<0))
  {
    // Memory ranging from 0..640KiB (in 1KiB units)
    iMemFree += mbi->mem_lower * 1024;
    // Memory ranging from 1MiB (in 1KiB units)
    iMemFree += mbi->mem_upper * 1024;
    
    printk("Lower memory: %dKiB\n", mbi->mem_lower);
    printk("Upper memory: %dKiB\n", mbi->mem_upper);
  }
  else
  {
    panic("ERROR: Can not detect usable memory");
  }

  if(iMemFree < (2 * 1024 * 1024))
  {
    panic("ERROR: %dKiB free memory, need at least 2MiB", (uint32_t)(iMemFree >> 10));
  }
  
  // Determine the top of the physical memory
  if(mbi->flags & (1<<6))
  {
    // Use bios memory map provided by GRUB
    // ------------------------------------
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
      if((mmap->type == 1) || (mmap->type == 3) || (mmap->type == 4))
      {
        if(iMemTop < (mmap->base_addr_low + mmap->length_low))
          iMemTop = mmap->base_addr_low + mmap->length_low;
      }
    }
  }
  else
  {
    printk("Warning: No BIOS memory map found\n");
    
    // Assume 1MiB + upper memory
    iMemTop = (1 * 1024 * 1024) + (mbi->mem_upper * 1024);
  }
  
  printk("Memory top: %dKiB\n", (uint32_t)(iMemTop >> 10));

  // Create memory map right after our kernel and loaded modules
  // 1 - Kernel location and size
  //printk("Kernel: start: %d, size: %d\n", (unsigned int)&start_text, (unsigned int)&end_bss - (unsigned int)&start_text);
  uint8_t * pMemoryMap = (uint8_t *)VIRT_TO_PHYS(&end_bss);
  // 2 - Module location and size
  if(mbi->flags & (1<<3))
  {
    module_t * mod = (module_t *) mbi->mods_addr;
    
    for(unsigned int i = 0; i < mbi->mods_count; i++, mod++)
    {
      //printk("Module: start: %d, size: %d\n", mod->mod_start, mod->mod_end - mod->mod_start);
      if((uint8_t *)mod->mod_end > pMemoryMap)
        pMemoryMap = (uint8_t *)mod->mod_end;
    }
  }
  pMemoryMap = (uint8_t *)PHYS_TO_VIRT(pMemoryMap);

  // Initialize memory map, all pages will be initially marked used
  init_pmm(pMemoryMap, iMemTop >> 12);

  // Free memory that multiboot/bios reports available
  if(mbi->flags & (1<<6))
  {
    for(memory_map_t * mmap = (memory_map_t *) mbi->mmap_addr; (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length; mmap = (memory_map_t *) ((unsigned long)mmap + mmap->size + sizeof (mmap->size)))
      if(mmap->type == 1)
        physFreeRange(mmap->base_addr_low, mmap->length_low);
  }
  else
  {
    // Free lower memory
    physFreeRange(0, mbi->mem_lower * 1024);
    // Free upper memory
    physFreeRange(1 * 1024 * 1024, mbi->mem_upper * 1024);
  }

  // Allocate interrupt area (start: 0, size: 4KiB)
  physAllocRange(0, 4*1024);
  // Allocate kernel
  physAllocRange(VIRT_TO_PHYS(&start_text), (unsigned int)&end_bss - (unsigned int)&start_text);
  // Allocate modules
  if((mbi->flags & (1<<3)) && ((int)mbi->mods_count > 0))
  {
    module_t * mod = (module_t *) mbi->mods_addr;
    for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
      physAllocRange(mod->mod_start, mod->mod_end - mod->mod_start);
  }
  // Allocate memory map
  // FIXME: Uses bytes instead of bits
  physAllocRange(VIRT_TO_PHYS(pMemoryMap), iMemTop >> 12);
  
  //printk("Memory Map: 0x%x + 0x%x = 0x%x\n", (uint32_t)pMemoryMap, (uint32_t)(iMemTop >> 12), (uint32_t)pMemoryMap + (uint32_t)(iMemTop >> 12));
}

// -----------------------------------------------------------------------------
void
init_cpu()
{
  // Activate the GDTR and reload the segment registers
  setGDTR(&cGDT.dtr_);
  setDS(selDataKernel);
  setSS(selDataKernel);
  setES(selDataKernel);
  setFS(selDataKernel);
  setGS(selDataKernel);
  // Activate the IDTR
  setIDTR(&cIDT.dtr_);
}

// -----------------------------------------------------------------------------
void
arch_main(unsigned long magic, multiboot_info_t * mbi)
{
#ifdef CONFIG_DEBUGGING
  cDebug.init();
  pDebug = &cDebug;
#endif // #ifdef CONFIG_DEBUGGING

  // ---------------------------------------
  // Miltiboot loader required
  if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    panic("ERROR: Multiboot loader information not present\n");
  }
  
  // Initialize the most essential data structures first
  init_memmap(mbi);
  init_gdt();
  init_idt();
  // Initialize CPU
  init_cpu();
  // Initialize CPU identification
  CPU::init();

  // Initialize legacy IRQ
  c8259.init();
  // Initialize legacy timer
  c8254.init();
  c8254.setTimerFrequency(100.0f);

  // ------------------------------------
  // Parse settings from the command line
  if(mbi->flags & (1<<2))
  {
    cSettings.parse((char *)mbi->cmdline);
  }

  // ---------------------
  // Setup Task Management
  // ---------------------
  task_init();

  // ------------
  // Load modules
  // ------------
  if((mbi->flags & (1<<3)) && ((int)mbi->mods_count > 0))
  {
    //printk("Found Modules: %d\n", (int)mbi->mods_count);
    module_t * mod = (module_t *) mbi->mods_addr;
    for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
    {
      CTask * pNewTask = NULL;//new CTask;

      printk("Loading: %s\n", (char *)mod->string);

      // Try to load as elf32 file
      if(loadELF32((void *)mod->mod_start, *pNewTask) == 0)
        printk(" - Done\n");
      else
        printk(" - Unknown File\n");
    }
  }

  // Initialize GPF handler task
  init_gpf();

  // Initialize PCI bus
  init_pci();

  // Initialize drivers
  init_drivers();

  // Enable interrupts
  local_irq_enable();
  printk("Interrupts...OK\n");

  printk("PC arch ready\n");
  //while(1);

  // We should never return from bricks_main
  bricks_main();
}
