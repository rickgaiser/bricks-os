#ifndef ASM_HEAP_H
#define ASM_HEAP_H


#include "kernel/memoryManager.h"
#include "stddef.h"


struct SHeap
{
  void * pStart;
  size_t iSize;
  SMemBlockHeader * pListHead;
};

extern SHeap heaps[];
extern const int iHeapCount;


#endif
