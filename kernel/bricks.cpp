// SYS include files
#include "kernel/bricks.h"
#include "kernel/memoryManager.h"
#include "asm/arch.h"
#include "asm/cpu.h"

// STL include files
#include "iostream"

// Posix include files
#include "unistd.h"
#include "pthread.h"


// -----------------------------------------------------------------------------
// Thread passes STDIN to STDOUT
void *
pipeThread(void * arg)
{
  std::cout<<"Thread Running"<<std::endl;

  // Forever print key presses
  char buffer[10];
  while(true)
  {
    if(::read(STDIN, buffer, 10) > 0)
      std::cout<<buffer;
  }

  return 0;
}

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

  // Create pipe thread, passing STDIN to STDOUT
  pthread_t thrPipe;
  if(pthread_create(&thrPipe, 0, pipeThread, 0) != 0)
    std::cout<<"ERROR: Unable to create thread!"<<std::endl;

  // Enable interrupts
  CCPU::sti();
  std::cout<<"Interrupts enabled"<<std::endl;

  // Halt current thread
  // FIXME: Forever consuming CPU time now!
  CCPU::halt();

  return 0;
}
