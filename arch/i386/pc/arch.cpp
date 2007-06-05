#include "kernel/bricks.h"
#include "kernel/task.h"
#include "kernel/elf.h"
#include "asm/irq.h"
#include "asm/cpu.h"
#include "video.h"
#include "keyboard.h"
#include "gdt.h"
#include "idt.h"
#include "hal.h"
#include "multiboot.h"
#include "string.h"
#include "iostream"


CIRQ             cIRQ;
CI386Video       cVideo;
CI386Keyboard    cKeyboard;
CGDT             cGDT;
CIDT             cIDT;

STaskStateSegment taskMain;
STaskStateSegment taskTest;
selector_t        selTaskMain;
selector_t        selTaskTest;
uint8_t           stack_pl0[512];
uint8_t           stack_pl3[512];


// -----------------------------------------------------------------------------
void
ttt()
{
  static const char * hello = "Hello Kernel!\n";
  
  while(true)
  {
    std::cout<<"test test test"<<std::endl;
    
    // Test system call
    sysCall1(0, hello);
    
    // Return to kernel
    jmpTask(selTaskMain);
  }
}

// -----------------------------------------------------------------------------
void
testTask()
{
  // Setup test task
  memset(&taskTest, 0, sizeof(STaskStateSegment));
  taskTest.esp0 = (uint32_t)&stack_pl0 + 512;
  taskTest.ss0  = selDataSys;
  taskTest.esp  = (uint32_t)&stack_pl3 + 512;
  taskTest.eip  = (uint32_t)ttt;
  taskTest.eflags = 0x200;
  taskTest.es   = selDataUser;
  taskTest.cs   = selCodeUser;
  taskTest.ss   = selDataUser;
  taskTest.ds   = selDataUser;
  taskTest.fs   = selDataUser;
  taskTest.gs   = selDataUser;
  
  selTaskMain = cGDT.createSegment(stTSS, 3, (uint32_t)&taskMain, sizeof(STaskStateSegment));
  selTaskTest = cGDT.createSegment(stTSS, 0, (uint32_t)&taskTest, sizeof(STaskStateSegment));

  // Set the current running tasks tss
  setTR(selTaskMain);
  
  // Jump to test task
  std::cout<<"Starting test task"<<std::endl;
  jmpTask(selTaskTest);
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
      for(unsigned int i(0); i < mbi->mods_count; i++, mod++)
      {
        std::cout<<" - "<<(char *)mod->string;
        
        if(loadELF32((void *)mod->mod_start) == 0)
        {
          std::cout<<" - ELF32 File"<<std::endl;
        }
        else if(loadELF64((void *)mod->mod_start) == 0)
        {
          std::cout<<" - ELF64 File"<<std::endl;
        }
        else
        {
          std::cout<<" - Unknown File: ";
          for(char * pMessage = (char *)mod->mod_start; pMessage < (char *)mod->mod_end; pMessage++)
            std::cout<<*pMessage;
        }
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
      printf("mmap_addr = 0x%x, mmap_length = 0x%x\n", (unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
      for(memory_map_t * mmap = (memory_map_t *) mbi->mmap_addr; (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length; mmap = (memory_map_t *) ((unsigned long) mmap + mmap->size + sizeof (mmap->size)))
	printf(" size = 0x%x, base_addr = 0x%x%x, length = 0x%x%x, type = 0x%x\n",
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
  
  testTask();

  return bricks_main();
}

