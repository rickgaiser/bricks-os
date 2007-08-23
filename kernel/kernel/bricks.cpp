// SYS include files
#include "kernel/bricks.h"
#include "asm/cpu.h"

// STL include files
#include "iostream"

// Posix include files
#include "unistd.h"
#include "pthread.h"


#ifdef CONFIG_MULTITASKING
// -----------------------------------------------------------------------------
void *
thread(void * arg)
{
  std::cout<<"Threads...OK"<<std::endl;

  return 0;
}
#endif // CONFIG_MULTITASKING

// -----------------------------------------------------------------------------
int
bricks_main()
{
  // Welcome user :-)
  std::cout<<"Bricks-OS"<<std::endl;
  std::cout<<"========="<<std::endl;

#ifdef CONFIG_MULTITASKING
  // Create thread
  pthread_t thr;
  if(pthread_create(&thr, 0, thread, 0) != 0)
    std::cout<<"ERROR: Unable to create thread!"<<std::endl;
#endif // CONFIG_MULTITASKING

  // Enable interrupts
  std::cout<<"Interrupts...";
  CCPU::sti();
  std::cout<<"OK"<<std::endl;

  // Halt current thread
  // FIXME: Forever consuming CPU time now!
  while(true)
  {
    CCPU::halt();
  }

  return 0;
}
