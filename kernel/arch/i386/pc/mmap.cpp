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


#include "mmap.h"
#include "string.h"
#include "kernel/debug.h"


uint8_t       * pMemMap = 0;
unsigned int    iPageCount(0);


// -----------------------------------------------------------------------------
void
init_mmap(uint8_t * addr, unsigned int pageCount)
{
  pMemMap    = addr;
  iPageCount = pageCount;

  memset(pMemMap, 1, iPageCount);
}

// -----------------------------------------------------------------------------
uint64_t
physAllocPageHigh()
{
  // Locate
  for(unsigned int index(272); index < iPageCount; index++)
  {
    if(pMemMap[index] == 0)
    {
      pMemMap[index] = 1;
      return (index << 12);
    }
  }

  panic("physAllocPageHigh: No more pages!\n");

  return 0;
}

// -----------------------------------------------------------------------------
uint32_t
physAllocPageLow()
{
  unsigned int LastPageIndex = (iPageCount > 256) ? 256 : iPageCount;

  // Locate
  for(unsigned int index(0); index < LastPageIndex; index++)
  {
    if(pMemMap[index] == 0)
    {
      pMemMap[index] = 1;
      return (index << 12);
    }
  }

  panic("physAllocPageLow: No more pages!\n");

  return 0;
}

// -----------------------------------------------------------------------------
void
physFreePage(uint64_t addr)
{
  addr >>= 12;

  if(addr < iPageCount)
    pMemMap[addr] = 0;
}

// -----------------------------------------------------------------------------
uint64_t
physAllocRange(uint64_t start, uint64_t length)
{
  unsigned int index = PAGE_ALIGN_DOWN_4K(start) >> 12;
  unsigned int end = PAGE_ALIGN_UP_4K(start + length) >> 12;

  // Validate range
  if(end > iPageCount)
    return 0;

  // Allocate
  while(index < end)
    pMemMap[index++] = 1;

  return start;
}

// -----------------------------------------------------------------------------
void
physFreeRange(uint64_t start, uint64_t length)
{
  unsigned int index = PAGE_ALIGN_DOWN_4K(start) >> 12;
  unsigned int end = PAGE_ALIGN_DOWN_4K(start + length) >> 12;

  // Validate range
  if(end > iPageCount)
    return;

  // Free
  while(index < end)
    pMemMap[index++] = 0;
}

// -----------------------------------------------------------------------------
unsigned int
usedPageCount()
{
  unsigned int count(0);

  for(unsigned int index(0); index < iPageCount; index++)
    if(pMemMap[index] != 0)
      count++;

  return count;
}

// -----------------------------------------------------------------------------
unsigned int
freePageCount()
{
  unsigned int count(0);

  for(unsigned int index(0); index < iPageCount; index++)
    if(pMemMap[index] == 0)
      count++;

  return count;
}

// -----------------------------------------------------------------------------
unsigned int
pageCount()
{
  return iPageCount;
}

