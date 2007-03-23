#include "kernel/memoryManager.h"


#ifdef GBA
extern char __iheap_start, __iwram_top;
extern char __eheap_start, __eheap_end;
SHeap heaps[] =
{
  {&__eheap_start, &__eheap_end - &__eheap_start}
 ,{&__iheap_start, &__iwram_top - &__iheap_start}
};
#endif // GBA

#ifdef NDS7
extern char _end, __iwram_top;
SHeap heaps[] =
{
  {&_end, &__iwram_top - &_end}
};
#endif // NDS7

#ifdef NDS9
extern char _end, __eheap_end;
SHeap heaps[] =
{
  {&_end, &__eheap_end - &_end}
};
#endif // NDS9

const int iHeapCount(sizeof(heaps) / sizeof(SHeap));
