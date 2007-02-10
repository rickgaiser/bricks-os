#include "kernel/memoryManager.h"


const size_t iMinBlockSize(sizeof(SMemBlockHeader) + 1);


// -----------------------------------------------------------------------------
void *
operator new(size_t size)
{
  return kmalloc(size);
}

// -----------------------------------------------------------------------------
void
operator delete(void * mem)
{
  kfree(mem);
}

// -----------------------------------------------------------------------------
int
CMemoryManager::init()
{
  for(int i(0) ; i < iHeapCount; i++)
  {
    SHeap * pHeap = &(heaps[i]);
    SMemBlockHeader * pHeader = (SMemBlockHeader *)pHeap->pStart;

    // Initialize the header
    pHeader->bAllocated = false;
    pHeader->iSize = pHeap->iSize;
    pHeader->pPrev = 0;
    pHeader->pNext = 0;

    pHeap->pListHead = pHeader;
  }

  return 0;    
}

// -----------------------------------------------------------------------------
void *
CMemoryManager::malloc(size_t size/*, SHeap * pHeap*/)
{
  SMemBlockHeader * pCurrent = heaps[0].pListHead; // pHeap->pListHead
  
  size += sizeof(SMemBlockHeader);
  
  // Locate free block
  while(true)
  {
    if((pCurrent->bAllocated == false) && (pCurrent->iSize >= size))
    {
      // Block found
      break;
    }
    else
    {
      // Advance to next block
      pCurrent = pCurrent->pNext;
      if(pCurrent == 0)
        return 0;
    }
  }
  
  // Setup the found block
  if((pCurrent->iSize - size) < iMinBlockSize)
  {
    // Use entire block
    // Setup current block
    pCurrent->bAllocated = true;
  }
  else
  {
    // Split block
    // Initialize new block
    SMemBlockHeader * pNew = (SMemBlockHeader *)(((int)pCurrent) + size);
    pNew->bAllocated = false;
    pNew->iSize = pCurrent->iSize - size;
    pNew->pPrev = pCurrent;
    pNew->pNext = pCurrent->pNext;
    // Setup current block
    pCurrent->bAllocated = true;
    pCurrent->iSize = size;
    pCurrent->pNext = pNew;
  }

  return (void *)(((int)pCurrent) + sizeof(SMemBlockHeader));
}

// -----------------------------------------------------------------------------
void
CMemoryManager::free(void * block)
{
  if(block == 0)
    return;

  SMemBlockHeader * pBlock = (SMemBlockHeader *)block;

  pBlock->bAllocated = false;
  
  // Merge with next block?
  SMemBlockHeader * pNext = pBlock->pNext;
  if(pNext != 0)
  {
    if(pNext->bAllocated == false)
    {
      // Merge blocks
      pBlock->iSize += pNext->iSize;
      pBlock->pNext = pNext->pNext;
      if(pNext->pNext != 0)
        pNext->pNext->pPrev = pBlock;
    }
  }

  // Merge with previous block?
  SMemBlockHeader * pPrev = pBlock->pNext;
  if(pPrev != 0)
  {
    if(pPrev->bAllocated == false)
    {
      // Merge blocks
      pPrev->iSize += pBlock->iSize;
      pPrev->pNext = pBlock->pNext;
      if(pBlock->pNext != 0)
        pBlock->pNext->pPrev = pPrev;
    }
  }
}
