#include "kernel/memoryManager.h"
#include "asm/irq.h"


extern char _end;
extern char _heap_size;

pt_regs * current_thread;
SHeap heaps[] =
{
  {&_end, 4 * 1024 * 1024}  // FIXME!!
};
const int iHeapCount(sizeof(heaps) / sizeof(SHeap));
