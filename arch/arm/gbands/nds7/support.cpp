#include "kernel/memoryManager.h"


extern char _end, __iwram_top;

SHeap heaps[] =
{
  {&_end, &__iwram_top - &_end, 0}
};
const int iHeapCount(sizeof(heaps) / sizeof(SHeap));
