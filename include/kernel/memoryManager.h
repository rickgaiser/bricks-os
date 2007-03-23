#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H


#include "stddef.h"


struct SHeap
{
  void * pStart;
  size_t iSize;
};


char *sbrk(int incr);
void * kmalloc(size_t size);
void kfree(void * block);
void * operator new(size_t size);
void operator delete(void * mem);


extern SHeap heaps[];
extern const int iHeapCount;


#endif
