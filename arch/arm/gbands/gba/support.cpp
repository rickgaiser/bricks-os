#include "kernel/memoryManager.h"


extern char __iheap_start, __iwram_top;
extern char __eheap_start, __eheap_end;

SHeap heaps[] =
{
  {&__eheap_start, &__eheap_end - &__eheap_start, 0}
 ,{&__iheap_start, &__iwram_top - &__iheap_start, 0}
};
const int iHeapCount(sizeof(heaps) / sizeof(SHeap));
