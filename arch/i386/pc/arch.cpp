#include "kernel/bricks.h"
#include "kernel/task.h"
#include "asm/irq.h"
#include "video.h"
#include "keyboard.h"
#include "gdt.h"
#include "idt.h"
#include "multiboot.h"
#include "iostream"


CIRQ             cIRQ;
CI386Video       cVideo;
CI386Keyboard    cKeyboard;
CGDT             cGDT;
CIDT             cIDT;


// -----------------------------------------------------------------------------
int
main(unsigned long magic, multiboot_info_t * mbi)
{
  int iRetVal(0);

  if(cGDT.init() == -1)
    iRetVal = -1;
  if(cIDT.init() == -1)
    iRetVal = -1;
  if(cIRQ.init() == -1)
    iRetVal = -1;
  if(cVideo.init() == -1)
    iRetVal = -1;
  if(cKeyboard.init() == -1)
    iRetVal = -1;

  CTaskManager::setStandardOutput(&cVideo);
  CTaskManager::setStandardInput(&cKeyboard);

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
    
    /*
    // Boot device
    if(mbi->flags & (1<<1))
    {
      //printf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);
    }
    */
    
    // Command line
    if(mbi->flags & (1<<2))
    {
      std::cout<<"Command line: "<<(char *)mbi->cmdline<<std::endl;
    }

    // Modules
    if(mbi->flags & (1<<3))
    {
      std::cout<<"Modules: "<<(int)mbi->mods_count<<std::endl;

      module_t * mod = (module_t *) mbi->mods_addr;
      for(unsigned int i(0); i < mbi->mods_count; i++)
      {
        std::cout<<" - "<<(char *)mod->string<<std::endl;
        
        for(char * pMessage = (char *)mod->mod_start; pMessage < (char *)mod->mod_end; pMessage++)
          std::cout<<*pMessage;
	mod++;
      }
    }

    /*
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

    // Memory Map
    if(mbi->flags & (1<<6))
    {
      memory_map_t *mmap;
      
      printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
	      (unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
      for (mmap = (memory_map_t *) mbi->mmap_addr;
	   (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
	   mmap = (memory_map_t *) ((unsigned long) mmap
				    + mmap->size + sizeof (mmap->size)))
	printf (" size = 0x%x, base_addr = 0x%x%x,"
		" length = 0x%x%x, type = 0x%x\n",
		(unsigned) mmap->size,
		(unsigned) mmap->base_addr_high,
		(unsigned) mmap->base_addr_low,
		(unsigned) mmap->length_high,
		(unsigned) mmap->length_low,
		(unsigned) mmap->type);
    }
    */
  }
  else
  {
    // Dirty Detector
    volatile unsigned int * pMem = (unsigned int *)((4*1024*1024) - sizeof(unsigned int));  // Start at 4MiB
    while(true)
    {
      unsigned int tmp = *pMem;
      *pMem = ~*pMem;
      if(*pMem != ~tmp)
        break;
      *pMem = tmp;
      pMem += 1024;
    }
    std::cout<<"Memory: "<<((unsigned int)pMem - 1024)/(1024*1024)<<"MiB"<<std::endl;
  }

  return bricks_main();
}

