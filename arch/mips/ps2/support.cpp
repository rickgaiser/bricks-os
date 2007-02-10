#include "kernel/bricks.h"
#include "kernel/memoryManager.h"


SHeap heaps[] =
{
};
const int iHeapCount(sizeof(heaps) / sizeof(SHeap));


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  return bricks_main();
}
