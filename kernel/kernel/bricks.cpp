#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "asm/arch/config.h"
#include "asm/cpu.h"


extern "C" int appMain(int argc, char * argv[]);


// -----------------------------------------------------------------------------
int
bricks_main()
{
  // Welcome user :-)
  printk("Bricks-OS\n");
  printk("=========\n");

  // Execute main application
  appMain(0, 0);

  // Halt current thread
  // FIXME: Forever consuming CPU time now!
  while(true)
  {
    ::halt();
  }

  return 0;
}
