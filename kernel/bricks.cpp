// SYS include files
#include "kernel/bricks.h"
#include "kernel/memoryManager.h"
#include "asm/arch.h"
#include "asm/cpu.h"

// STL include files
#include "iostream"

// Posix include files
#include "unistd.h"


// -----------------------------------------------------------------------------
int
bricks_main()
{
  // Initialize the memory manager so we can use new/delete/malloc/free
  CMemoryManager::init();

  // Initialize architecture first
  if(arch::init() == -1)
    std::cout<<"Arch...ERROR"<<std::endl;
  else
    std::cout<<"Arch...OK"<<std::endl;

  // Welcome user :-)
  std::cout<<"Bricks-OS"<<std::endl;
  std::cout<<"========="<<std::endl;

  // Enable interrupts
  CCPU::sti();
  std::cout<<"Interrupts enabled"<<std::endl;

  // Forever print key presses
  char buffer[10];
  while(true)
  {
    if(::read(STDIN, buffer, 10) > 0)
      std::cout<<"key pressed: "<<buffer<<std::endl;
  }

  return 0;
}
