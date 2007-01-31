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

class CMemoryManager
{
public:
  static int init();
  static void * malloc(size_t size/*, SHeap * heap*/);
  static void free(void * block);

private:
  CMemoryManager(){}
};


#endif
