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

  // Initialize the memory manager so we can use new/delete/malloc/free
  //init_heap(&__heap_start, (uint32_t)(&__heap_end - &__heap_start));

#ifdef CONFIG_DEBUGGING
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

//  printf("Bricks-OS\n");
//  while(1);

  return bricks_main();
}
