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


#include "asm/aspace.h"
#include "mmap.h"
#include "string.h"


extern bool bPAEEnabled;


// -----------------------------------------------------------------------------
CAddressSpace::CAddressSpace()
{
  if(bPAEEnabled == true)
  {
    // Locate 16KiB of phisical memory for page directory entries
    PDPT_.entry[0] = physAllocPageHigh() | 1;
    PDPT_.entry[1] = physAllocPageHigh() | 1;
    PDPT_.entry[2] = physAllocPageHigh() | 1;
    PDPT_.entry[3] = physAllocPageHigh() | 1;

    // Setup PDEs
    pde64_t * pPDE;
    pPDE = (pde64_t *)(PDPT_.entry[0] & 0xfffff000);
    memset(pPDE, 0, 4096);
    pPDE = (pde64_t *)(PDPT_.entry[1] & 0xfffff000);
    memset(pPDE, 0, 4096);
    pPDE = (pde64_t *)(PDPT_.entry[2] & 0xfffff000);
    memset(pPDE, 0, 4096);
    pPDE = (pde64_t *)(PDPT_.entry[3] & 0xfffff000);
    memset(pPDE, 0, 4096);
    // Map PDE into itself in the last entries, making it accessible in the last 8MiB of virtual memory
    pPDE[508] = PDPT_.entry[0] | PG_USER | PG_WRITABLE | PG_PRESENT;
    pPDE[509] = PDPT_.entry[1] | PG_USER | PG_WRITABLE | PG_PRESENT;
    pPDE[510] = PDPT_.entry[2] | PG_USER | PG_WRITABLE | PG_PRESENT;
    pPDE[511] = PDPT_.entry[3] | PG_USER | PG_WRITABLE | PG_PRESENT;

    iCR3_ = (uint32_t)&PDPT_;
  }
  else
  {
    // Locate 4KiB of phisical memory for page directory entries
    pPD_ = (pde32_t *)physAllocPageHigh();
    memset((void *)pPD_, 0, 4096);
    // Map PDE into itself in the last entries, making it accessible in the last 4MiB of virtual memory
    pPD_[1023] = (uint32_t)pPD_ | PG_USER | PG_WRITABLE | PG_PRESENT;

    iCR3_ = (uint32_t)pPD_;
  }

  // Map kernel into address space: identity map bottom 4MiB
  mapIdentity(0, 0x00400000);
}

// -----------------------------------------------------------------------------
CAddressSpace::~CAddressSpace()
{
  if(bPAEEnabled == true)
  {
    physFreePage(PDPT_.entry[0] & 0xfffff000);
    physFreePage(PDPT_.entry[1] & 0xfffff000);
    physFreePage(PDPT_.entry[2] & 0xfffff000);
    physFreePage(PDPT_.entry[3] & 0xfffff000);
  }
  else
  {
    physFreePage((uint64_t)pPD_);
  }
}

// -----------------------------------------------------------------------------
void
CAddressSpace::map(void * start, uint32_t length)
{
  uint32_t iStart(PAGE_ALIGN_DOWN_4K((uint32_t)start) >> 12);
  uint32_t iCount(PAGE_ALIGN_UP_4K((uint32_t)length) >> 12);

  if(bPAEEnabled == true)
  {
    pde64_t * pPD;
    pte64_t * pPT;

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdppidx = (idx & 0x000c0000) >> 20;
      uint32_t pdidx   = (idx & 0x0003fe00) >>  9;
      uint32_t ptidx   = (idx & 0x000001ff);

      // Get PD
      pPD = (pde64_t *)(PDPT_.entry[pdppidx] & 0xfffff000);

      // Create PT if not present
      if((pPD[pdidx] & PG_PRESENT) == 0)
      {
        pPT = (pte64_t *)physAllocPageHigh();
        memset(pPT, 0, 4096);
        pPD[pdidx] = (uint64_t)pPT | PG_USER | PG_WRITABLE | PG_PRESENT;
      }

      // Get PT
      pPT = (pte64_t *)(pPD[pdidx] & 0xfffff000);

      // Map new empty page if not present
      if((pPT[ptidx] & PG_PRESENT) == 0)
      {
        pPT[ptidx] = physAllocPageHigh() | PG_USER | PG_WRITABLE | PG_PRESENT;
      }
    }
  }
  else
  {
    pte32_t * pPT;

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdidx = (idx & 0x000ffc00) >> 10;
      uint32_t ptidx = (idx & 0x000003ff);

      // Create PT if not present
      if((pPD_[pdidx] & PG_PRESENT) == 0)
      {
        pPT = (pte32_t *)physAllocPageHigh();
        memset(pPT, 0, 4096);
        pPD_[pdidx] = (uint32_t)pPT | PG_USER | PG_WRITABLE | PG_PRESENT;
      }

      // Get PT
      pPT = (pte32_t *)(pPD_[pdidx] & 0xfffff000);

      // Map new empty page if not present
      if((pPT[ptidx] & PG_PRESENT) == 0)
      {
        pPT[ptidx] = physAllocPageHigh() | PG_USER | PG_WRITABLE | PG_PRESENT;
      }
    }
  }
}

// -----------------------------------------------------------------------------
void
CAddressSpace::mapIdentity(void * start, uint32_t length)
{
  uint32_t iStart(PAGE_ALIGN_DOWN_4K((uint32_t)start) >> 12);
  uint32_t iCount(PAGE_ALIGN_UP_4K((uint32_t)length) >> 12);

  if(bPAEEnabled == true)
  {
    pde64_t * pPD;
    pte64_t * pPT;

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdppidx = (idx & 0x000c0000) >> 20;
      uint32_t pdidx   = (idx & 0x0003fe00) >>  9;
      uint32_t ptidx   = (idx & 0x000001ff);

      // Get PD
      pPD = (pde64_t *)(PDPT_.entry[pdppidx] & 0xfffff000);

      // Create PT if not present
      if((pPD[pdidx] & PG_PRESENT) == 0)
      {
        pPT = (pte64_t *)physAllocPageHigh();
        memset(pPT, 0, 4096);
        pPD[pdidx] = (uint64_t)pPT | PG_USER | PG_WRITABLE | PG_PRESENT;
      }

      // Get PT
      pPT = (pte64_t *)(pPD[pdidx] & 0xfffff000);

      // Map Page
      pPT[ptidx] = (idx << 12) | PG_USER | PG_WRITABLE | PG_PRESENT;
    }
  }
  else
  {
    pte32_t * pPT;

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdidx = (idx & 0x000ffc00) >> 10;
      uint32_t ptidx = (idx & 0x000003ff);

      // Create PT if not present
      if((pPD_[pdidx] & PG_PRESENT) == 0)
      {
        pPT = (pte32_t *)physAllocPageHigh();
        memset(pPT, 0, 4096);
        pPD_[pdidx] = (uint32_t)pPT | PG_USER | PG_WRITABLE | PG_PRESENT;
      }

      // Get PT
      pPT = (pte32_t *)(pPD_[pdidx] & 0xfffff000);

      // Map Page
      pPT[ptidx] = (idx << 12) | PG_USER | PG_WRITABLE | PG_PRESENT;
    }
  }
}

// -----------------------------------------------------------------------------
// Copy a range of PD entries from an address space to this one
void
CAddressSpace::mapShared(const CAddressSpace & as, void * start, uint32_t length)
{
  uint32_t iStart;
  uint32_t iCount;

  if(bPAEEnabled == true)
  {
    pde64_t * pPD1;
    pde64_t * pPD2;

    iStart = PAGE_ALIGN_DOWN_4K((uint32_t)start) >> (12 + 9);
    iCount = PAGE_ALIGN_UP_4K((uint32_t)length) >> (12 + 9);

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    {
      uint32_t pdppidx = (idx & 0x00000600) >> 11;
      uint32_t pdidx   = (idx & 0x000001ff);

      // Get PD
      pPD1 = (pde64_t *)(PDPT_.entry[pdppidx] & 0xfffff000);
      pPD2 = (pde64_t *)(as.PDPT_.entry[pdppidx] & 0xfffff000);

      // Create PT if not present
      pPD1[pdidx] = pPD2[pdidx];
    }
  }
  else
  {
    iStart = PAGE_ALIGN_DOWN_4K((uint32_t)start) >> (12 + 10);
    iCount = PAGE_ALIGN_UP_4K((uint32_t)length) >> (12 + 10);

    for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
      pPD_[idx] = as.pPD_[idx];
  }
}
