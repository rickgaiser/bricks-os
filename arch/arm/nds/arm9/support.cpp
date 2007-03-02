#include "kernel/bricks.h"
#include "kernel/memoryManager.h"


extern char _end, __eheap_end;

SHeap heaps[] =
{
  {&_end, &__eheap_end - &_end, 0}
};
const int iHeapCount(sizeof(heaps) / sizeof(SHeap));


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  return bricks_main();
}
