#include "kernel/bricks.h"
#include "kernel/memoryManager.h"


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  // Initialize the memory manager so we can use new/delete/malloc/free
  CMemoryManager::init();

  return bricks_main();
}
