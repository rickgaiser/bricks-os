#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H


#include "stddef.h"


#define kmalloc CMemoryManager::malloc
#define kfree   CMemoryManager::free


struct SMemBlockHeader
{
  bool bAllocated;
  size_t iSize;

  // List
  SMemBlockHeader * pPrev;
  SMemBlockHeader * pNext;
};

struct SHeap
{
  void * pStart;
  size_t iSize;
  SMemBlockHeader * pListHead;
};

class CMemoryManager
{
public:
  static int init();
  static void * malloc(size_t size/*, SHeap * heap*/);
  static void free(void * block);

private:
  CMemoryManager(){}
};


extern SHeap heaps[];
extern const int iHeapCount;


#endif
