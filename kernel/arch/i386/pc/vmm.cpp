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


#include "asm/vmm.h"
#include "kernel/debug.h"
#include "pmm.h"
#include "string.h"


extern bool bPAEEnabled;


struct SDataBlock
{
  vaddress_t size;
};

struct SWindow
{
  vaddress_t base;
  vaddress_t size;
};

// Map "SDataBlock" inside "SWindow"
struct SMapping
{
  SDataBlock * data;
  SWindow    * window;
};

struct SHeap
{
  SMapping * mapping;
  vaddress_t size;
};


SDataBlock kernelHeapData    = {0};
SWindow    kernelHeapWindow  = {MM_KERNEL_HEAP_BASE, MM_KERNEL_HEAP_SIZE};
SMapping   kernelHeapMapping = {&kernelHeapData, &kernelHeapWindow};
SHeap      kernelHeap        = {&kernelHeapMapping, 0};


//---------------------------------------------------------------------------
// Enlarge heap by inc
char *
gen_sbrk(SHeap & heap, size_t inc)
{
  char * pReturn = (char *)(-1);

  // First we need to know if the new size fits the memory region
  if((heap.size + inc) <= heap.mapping->window->size)
  {
    // Allocate new pages
    while((heap.size + inc) > heap.mapping->data->size)
    {
      vmmMapPage(heap.mapping->window->base + heap.mapping->data->size, PTE_FLAG_WRITABLE | PTE_FLAG_GLOBAL);
      heap.mapping->data->size += PAGE_SIZE;
    }

    pReturn = (char *)(heap.mapping->window->base + heap.size);
    heap.size += inc;
  }

  return pReturn;
}

//---------------------------------------------------------------------------
char *
k_sbrk(size_t inc)
{
  printk("k_sbrk(%d)\n", inc);
  
  return gen_sbrk(kernelHeap, inc);
}

//---------------------------------------------------------------------------
// 386 has no invlpg instruction
//#define CONFIG_INVLPG
//---------------------------------------------------------------------------
static inline void
tlbFlushAll()
{
  setCR3(getCR3());
}

//---------------------------------------------------------------------------
static inline void
tlbFlushPTE(uint32_t vaddr)
{
#ifdef CONFIG_INVLPG
  // NOTE: invlpg is for >=486
  __asm__ volatile("invlpg %0"::"m" (*(char *)vaddr));
#else
  tlbFlushAll();
#endif
}

//---------------------------------------------------------------------------
static inline void
tlbFlushPDE(uint32_t vaddr)
{
  for(uint32_t i = 0; i < 1024; i++)
  {
    tlbFlushPTE(vaddr);
    vaddr += 4096;
  }
}

// -----------------------------------------------------------------------------
unsigned long
vmmGetPTE(vaddress_t vaddr)
{
  phys_page_addr_t retval = 0;
  unsigned long pd_index = vaddr >> 22;

  if((pPDCurrent[pd_index] & PDE_FLAG_PRESENT) == PDE_FLAG_PRESENT)
  {
    if((pPDCurrent[pd_index] & PDE_FLAG_LARGE_SIZE) == PDE_FLAG_LARGE_SIZE)
    {
      return pPDCurrent[pd_index];
    }
    else
    {
      unsigned long pt_index = vaddr >> 12;
      
      if((pPTCurrent[pt_index] & PTE_FLAG_PRESENT) == PTE_FLAG_PRESENT)
        retval = pPTCurrent[pt_index];
    }
  }
      
  return retval;
}

// -----------------------------------------------------------------------------
void
vmmMapPage(phys_page_addr_t paddr, vaddress_t vaddr, unsigned long flags)
{
  unsigned long pt_index = vaddr >> 12;
  unsigned long pd_index = vaddr >> 22;
  
  printk("Allocating a page at addr 0x%x\n", vaddr);
  
  // Allocate page directory if not yet present
  if((pPDCurrent[pd_index] & PDE_FLAG_PRESENT) != PDE_FLAG_PRESENT)
  {
    printk("Allocating a page table for addr 0x%x\n", vaddr);
    
    // Allocate new page table
    pPDCurrent[pd_index] = physAllocPage() | PDE_FLAG_WRITABLE | PDE_FLAG_USER | PTE_FLAG_GLOBAL | PDE_FLAG_PRESENT;
    // Clear the page table
    memset((void *)&pPTCurrent[pd_index<<10], 0, PAGE_SIZE);
  }
  
  // Don't allocate already allocated memory
  if((pPTCurrent[pt_index] & PTE_FLAG_PRESENT) == PTE_FLAG_PRESENT)
  {
    panic("Allocating an already allocated page");
  }

  // Allocate page
  pPTCurrent[pt_index] = (paddr & ~0xfff) | (flags & 0xfff) | PTE_FLAG_PRESENT;
  tlbFlushPTE(vaddr);
}

// -----------------------------------------------------------------------------
void
vmmMapPage(vaddress_t vaddr, unsigned long flags)
{
  unsigned long pt_index = vaddr >> 12;
  unsigned long pd_index = vaddr >> 22;
  
  printk("Allocating a page at addr 0x%x\n", vaddr);
  
  // Allocate page directory if not yet present
  if((pPDCurrent[pd_index] & PDE_FLAG_PRESENT) != PDE_FLAG_PRESENT)
  {
    printk("Allocating a page table for addr 0x%x\n", vaddr);
    
    // Allocate new page table
    pPDCurrent[pd_index] = physAllocPage() | PDE_FLAG_WRITABLE | PDE_FLAG_USER | PTE_FLAG_GLOBAL | PDE_FLAG_PRESENT;
    // Clear the page table
    memset((void *)&pPTCurrent[pd_index<<10], 0, PAGE_SIZE);
  }
  
  // Don't allocate already allocated memory
  if((pPTCurrent[pt_index] & PTE_FLAG_PRESENT) == PTE_FLAG_PRESENT)
  {
    panic("Allocating an already allocated page");
  }

  // Allocate page
  pPTCurrent[pt_index] = physAllocPage() | (flags & 0xfff) | PTE_FLAG_PRESENT;
  tlbFlushPTE(vaddr);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CBackingStore::CBackingStore(uint32_t start, uint32_t size)
 : iStart_(start)
 , iSize_(size)
{
}

// -----------------------------------------------------------------------------
CBackingStore::~CBackingStore()
{
}

// -----------------------------------------------------------------------------
void
CBackingStore::load(uint32_t dst, uint32_t src_offset, uint32_t size)
{
  if(src_offset > iSize_)
    panic("CBackingStore: src out of range");
  
  if((src_offset+size) > iSize_)
    size = iSize_ - src_offset;
    
  memcpy((char *)dst, (char *)(iStart_ + src_offset), size);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CArea::CArea(uint32_t _start, uint32_t _size, uint32_t _flags)
 : pAreaNext_(NULL)
 , pAreaPrev_(NULL)
{
  start(_start);
  size(_size);
  flags(_flags);
}

// -----------------------------------------------------------------------------
CArea::~CArea()
{
}

// -----------------------------------------------------------------------------
void
CArea::map(uint32_t vaddr)
{
  vmmMapPage(vaddr & ~0xfff, iArchFlags_);

  if(pBackingStore_ != NULL)
    pBackingStore_->load(vaddr, vaddr - iStart_, PAGE_SIZE);
  else
    memset((char *)vaddr, 0, PAGE_SIZE);
}

// -----------------------------------------------------------------------------
void
CArea::mapAll()
{
  uint32_t iStart(PAGE_ALIGN_DOWN_4K(iStart_) >> 12);
  uint32_t iCount(PAGE_ALIGN_UP_4K(iSize_) >> 12);
  
  for(uint32_t idx(iStart); idx < (iStart + iCount); idx++)
    map(idx<<12);
}

// -----------------------------------------------------------------------------
void
CArea::start(uint32_t start)
{
  iStart_ = start;
}

// -----------------------------------------------------------------------------
uint32_t
CArea::start()
{
  return iStart_;
}

// -----------------------------------------------------------------------------
void
CArea::size(uint32_t size)
{
  iSize_ = size;
}

// -----------------------------------------------------------------------------
uint32_t
CArea::size()
{
  return iSize_;
}

// -----------------------------------------------------------------------------
void
CArea::flags(uint32_t flags)
{
  iFlags_ = flags;
  
  iArchFlags_ = 0;
  if(iFlags_ & AREA_FLAG_WRITE)  iArchFlags_ |= PTE_FLAG_WRITABLE;
  if(iFlags_ & AREA_FLAG_USER)   iArchFlags_ |= PTE_FLAG_USER;
  if(iFlags_ & AREA_FLAG_GLOBAL) iArchFlags_ |= PTE_FLAG_GLOBAL;
}

// -----------------------------------------------------------------------------
uint32_t
CArea::flags()
{
  return iFlags_;
}

// -----------------------------------------------------------------------------
void
CArea::backingStore(CBackingStore * bs)
{
  pBackingStore_ = bs;
}

// -----------------------------------------------------------------------------
CBackingStore *
CArea::backingStore()
{
  return pBackingStore_;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern pde32_t initial_page_directory;
// -----------------------------------------------------------------------------
CAddressSpace::CAddressSpace()
 : pAreaFirst_(NULL)
 , pAreaLast_(NULL)
{
  iCR3_ = VIRT_TO_PHYS(&initial_page_directory);
}

// -----------------------------------------------------------------------------
CAddressSpace::~CAddressSpace()
{
}

// -----------------------------------------------------------------------------
void
CAddressSpace::map(CArea * area)
{
  // Add to area list
  areaPushBack(area);
}

// -----------------------------------------------------------------------------
CArea *
CAddressSpace::findArea(uint32_t vaddr)
{
  CArea * pFoundArea = NULL;
  
  for(CArea * a = pAreaFirst_; a != NULL; a = a->pAreaNext_)
  {
    if((a->start() <= vaddr) && ((a->start() + a->size()) > vaddr))
    {
      pFoundArea = a;
      break;
    }
  }
  
  return pFoundArea;
}

// -----------------------------------------------------------------------------
void
CAddressSpace::areaPushBack(CArea * area)
{
  if(pAreaLast_ == NULL)
  {
    pAreaFirst_ = area;
    pAreaLast_  = area;
  }
  else
  {
    pAreaLast_->pAreaNext_ = area;
    area->pAreaPrev_ = pAreaLast_;
    pAreaLast_ = area;
  }  
}

