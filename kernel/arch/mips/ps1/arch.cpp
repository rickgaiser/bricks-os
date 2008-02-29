#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "asm/arch/config.h"


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  //init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

  //printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  printk("PS1 arch ready\n");

  return bricks_main();
}
