/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


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
