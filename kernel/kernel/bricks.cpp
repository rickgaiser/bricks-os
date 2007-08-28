#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "asm/cpu.h"
#include "pthread.h"


#ifdef CONFIG_MULTITASKING
// -----------------------------------------------------------------------------
void *
thread(void * arg)
{
  printk("Threads...OK\n");

  return 0;
}
#endif // CONFIG_MULTITASKING

// -----------------------------------------------------------------------------
int
bricks_main()
{
  // Welcome user :-)
  printk("Bricks-OS\n");
  printk("=========\n");

#ifdef CONFIG_MULTITASKING
  // Create thread
  pthread_t thr;
  if(pthread_create(&thr, 0, thread, 0) != 0)
    printk("ERROR: Unable to create thread!\n");
#endif // CONFIG_MULTITASKING

  // Enable interrupts
  printk("Interrupts...\n");
  CCPU::sti();
  printk("Interrupts...OK\n");

  // Halt current thread
  // FIXME: Forever consuming CPU time now!
  while(true)
  {
    CCPU::halt();
  }

  return 0;
}
