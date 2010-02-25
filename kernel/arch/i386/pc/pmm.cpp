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


#include "pmm.h"
#include "string.h"
#include "kernel/debug.h"


uint8_t * pMemMap = 0;
phys_page_index_t iPageCount = 0;


// -----------------------------------------------------------------------------
// Initialize the physical memory bitmap
//  - Set all pages to used by default
// FIXME: Uses bytes instead of bits
void
init_pmm(uint8_t * addr, unsigned int pageCount)
{
  pMemMap    = addr;
  iPageCount = pageCount;

  memset(pMemMap, 1, iPageCount);
}

// -----------------------------------------------------------------------------
phys_page_addr_t
_physAllocPage(phys_page_index_t from, phys_page_index_t to)
{
  for(phys_page_index_t index = from; index < to; index++)
  {
    if(pMemMap[index] == 0)
    {
      pMemMap[index] = 1;
      return (index << 12);
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
phys_page_addr_t
physAllocPageUpper()
{
  return _physAllocPage(256, iPageCount);
}

// -----------------------------------------------------------------------------
phys_page_addr_t
physAllocPageLower()
{
  return _physAllocPage(0, 256);
}

// -----------------------------------------------------------------------------
phys_page_addr_t
physAllocPage()
{
  phys_page_addr_t pageAddr = physAllocPageUpper();
  
  if(pageAddr == 0)
    pageAddr = physAllocPageLower();
  
  if(pageAddr == 0)
    panic("ERROR: Out of memory");

  return pageAddr;
}

// -----------------------------------------------------------------------------
phys_page_addr_t
_physAllocSize(phys_mem_size_t length, phys_page_index_t from, phys_page_index_t to)
{
  phys_page_index_t iPageCount = length >> 12;
  phys_page_index_t iPagesFound = 0;
  phys_page_index_t iStartIndex = 0;
  
  for(phys_page_index_t index = from; index < to; index++)
  {
    if(pMemMap[index] == 0)
    {
      if(iPagesFound == 0)
        iStartIndex = index;
        
      iPagesFound++;
      if(iPagesFound >= iPageCount)
        return physAllocRange(iStartIndex << 12, length);
    }
    else
      iPagesFound = 0;
  }
  
  return 0;
}

// -----------------------------------------------------------------------------
phys_page_addr_t
physAllocSizeUpper(phys_mem_size_t length)
{
  return _physAllocSize(length, 256, iPageCount);
}

// -----------------------------------------------------------------------------
phys_page_addr_t
physAllocSizeLower(phys_mem_size_t length)
{
  return _physAllocSize(length, 0, 256);
}

// -----------------------------------------------------------------------------
phys_page_addr_t
physAllocSize(phys_mem_size_t length)
{
  phys_page_addr_t pageAddr = physAllocSizeUpper(length);
  
  if(pageAddr == 0)
    pageAddr = physAllocSizeLower(length);
  
  if(pageAddr == 0)
    panic("ERROR: Out of memory");
  
  return pageAddr;
}

// -----------------------------------------------------------------------------
phys_page_addr_t
physAllocRange(phys_page_addr_t addr, phys_mem_size_t length)
{
  phys_page_index_t pageStart = PAGE_ALIGN_DOWN_4K(addr) >> 12;
  phys_page_index_t pageEnd   = PAGE_ALIGN_UP_4K(addr + length) >> 12;

  // Validate range
  if(pageStart >= iPageCount)
    return 0;
  if(pageEnd >= iPageCount)
    return 0;

  // Allocate
  while(pageStart < pageEnd)
    pMemMap[pageStart++] = 1;

  return addr;
}

// -----------------------------------------------------------------------------
void
physFreePage(phys_page_addr_t addr)
{
  phys_page_index_t page = addr >> 12;

  if(page < iPageCount)
    pMemMap[page] = 0;
}

// -----------------------------------------------------------------------------
void
physFreeRange(phys_page_addr_t addr, phys_mem_size_t length)
{
  phys_page_index_t pageStart = PAGE_ALIGN_DOWN_4K(addr) >> 12;
  phys_page_index_t pageEnd   = PAGE_ALIGN_DOWN_4K(addr + length) >> 12;

  // Validate range
  if(pageStart >= iPageCount)
    return;
  if(pageEnd >= iPageCount)
    return;

  // Free
  while(pageStart < pageEnd)
    pMemMap[pageStart++] = 0;
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

