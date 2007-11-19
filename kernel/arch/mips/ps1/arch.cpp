#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  // Initialize the memory manager so we can use new/delete/malloc/free
  //init_heap(&__heap_start, (uint32_t)(&__heap_end - &__heap_start));

  return bricks_main();
}
