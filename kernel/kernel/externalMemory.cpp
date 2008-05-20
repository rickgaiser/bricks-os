#include "kernel/externalMemory.h"


//---------------------------------------------------------------------------
CExternalMemory::CExternalMemory(uint32_t base, uint32_t size, uint32_t blockSize)
 : base_(base)
 , size_(size)
 , blockSize_(blockSize)
{
  TAILQ_INIT(&queueHead_);

  SExternalMemoryBlock * pNew;

  pNew = new SExternalMemoryBlock;
  pNew->base      = base_;
  pNew->size      = size_;
  pNew->allocated = false;
  TAILQ_INSERT_TAIL(&queueHead_, pNew, queueEntry);
}

//---------------------------------------------------------------------------
CExternalMemory::~CExternalMemory()
{
  SExternalMemoryBlock * pAllocation;
  SExternalMemoryBlock * pSafe;

  TAILQ_FOREACH_SAFE(pAllocation, &queueHead_, queueEntry, pSafe)
  {
    delete pAllocation;
  }
}

//---------------------------------------------------------------------------
void *
CExternalMemory::malloc(uint32_t size)
{
  void * pReturn = NULL;
  SExternalMemoryBlock * pAllocation;
  SExternalMemoryBlock * pSafe;

  TAILQ_FOREACH_SAFE(pAllocation, &queueHead_, queueEntry, pSafe)
  {
    if((pAllocation->allocated == false) && (pAllocation->size >= size))
    {
      pReturn = (void *)(pAllocation->base);
      pAllocation->allocated = true;

      // Split up the entry?
      if((pAllocation->size - size) >= blockSize_)
      {
        // Create new entry
        SExternalMemoryBlock * pNew = new SExternalMemoryBlock;
        pNew->base      = pAllocation->base + size;
        pNew->size      = pAllocation->size - size;
        pNew->allocated = false;
        TAILQ_INSERT_AFTER(&queueHead_, pAllocation, pNew, queueEntry);

        // Edit existing entry
        pAllocation->size -= pNew->size;
      }

      break;
    }
  }

  return pReturn;
}

//---------------------------------------------------------------------------
void
CExternalMemory::free(void * base)
{
  SExternalMemoryBlock * pAllocation;
  SExternalMemoryBlock * pSafe;

  TAILQ_FOREACH_SAFE(pAllocation, &queueHead_, queueEntry, pSafe)
  {
    if(pAllocation->base == (uint32_t)base)
    {
      SExternalMemoryBlock * pTemp;

      pAllocation->allocated = false;

      // Merge with next entry?
      pTemp = TAILQ_NEXT(pAllocation, queueEntry);
      if((pTemp != NULL) && (pTemp->allocated == false))
      {
        pAllocation->size += pTemp->size;
        TAILQ_REMOVE(&queueHead_, pTemp, queueEntry);
        delete pTemp;
      }

      // Merge with previous entry?
      pTemp = TAILQ_PREV(pAllocation, SExternalMemoryBlockQueue, queueEntry);
      if((pTemp != NULL) && (pTemp->allocated == false))
      {
        // Merge
        pTemp->size += pAllocation->size;
        TAILQ_REMOVE(&queueHead_, pAllocation, queueEntry);
        delete pAllocation;
      }

      break;
    }
  }
}
