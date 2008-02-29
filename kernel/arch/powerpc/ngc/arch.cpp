#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "asm/arch/config.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING


#ifdef CONFIG_DEBUGGING
CNGCDebugScreen cDebug;
#endif // CONFIG_DEBUGGING


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  //init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

#ifdef CONFIG_DEBUGGING
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

  //printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  printk("NGC arch ready\n");

  return bricks_main();
}
