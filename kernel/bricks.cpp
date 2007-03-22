// SYS include files
#include "kernel/bricks.h"
#include "asm/cpu.h"

// STL include files
#include "iostream"

// Posix include files
#include "unistd.h"
#include "pthread.h"


#ifdef CONFIG_BWM
extern int bwm(int argc, char * argv[]);
#endif // CONFIG_BWM


#ifdef CONFIG_MULTITASKING
// -----------------------------------------------------------------------------
// Thread passes STDIN to STDOUT
void *
pipeThread(void * arg)
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
  pthread_t thrPipe;
  if(pthread_create(&thrPipe, 0, pipeThread, 0) != 0)
    std::cout<<"ERROR: Unable to create thread!"<<std::endl;
#endif // CONFIG_MULTITASKING

  // Enable interrupts
  std::cout<<"Interrupts...";
  CCPU::sti();
  std::cout<<"OK"<<std::endl;

#ifdef CONFIG_BWM
  // Wait for user input
  std::cout<<"   Press any key to start"<<std::endl;
  std::cout<<"-=Bricks-OS Window Manager=-"<<std::endl;
  char buffer[10];
  while(::read(STDIN, buffer, 10) <= 0){}
  bwm(0, 0);
#endif // CONFIG_BWM

  // Halt current thread
  // FIXME: Forever consuming CPU time now!
  CCPU::halt();

  return 0;
}
