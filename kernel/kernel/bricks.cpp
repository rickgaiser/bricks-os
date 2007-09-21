#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "asm/cpu.h"
#include "pthread.h"


extern int srrTest(int argc, char * argv[]);


// -----------------------------------------------------------------------------
int
bricks_main()
{
  // Welcome user :-)
  printk("Bricks-OS\n");
  printk("=========\n");

  // Enable interrupts
  printk("Interrupts...\n");
  local_irq_enable();
  printk("Interrupts...OK\n");

  // Execute application
  srrTest(0, 0);

  // Halt current thread
  // FIXME: Forever consuming CPU time now!
  while(true)
  {
    ::halt();
  }

  return 0;
}
