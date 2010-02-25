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


#ifndef I386_VMM_H
#define I386_VMM_H


#include "hal.h"
#include "inttypes.h"
#include "stddef.h"
#include "../../../arch/i386/pc/pmm.h"


// Virtual Address Type
typedef uint32_t vaddress_t;


// sbrk for kernel heap
char * k_sbrk(size_t inc);
// Map one page
void vmmMapPage(phys_page_addr_t paddr, vaddress_t vaddr, unsigned long flags);
// Map random page
void vmmMapPage(vaddress_t vaddr, unsigned long flags);


// -----------------------------------------------------------------------------
// Area flags
#define AREA_FLAG_READ    (1<<0)
#define AREA_FLAG_WRITE   (1<<1)
#define AREA_FLAG_EXECUTE (1<<2)
#define AREA_FLAG_USER    (1<<3)
#define AREA_FLAG_GLOBAL  (1<<4) // Only for operating system


// -----------------------------------------------------------------------------
// Early declarations
class CAddressSpace;


// -----------------------------------------------------------------------------
class CBackingStore
{
public:
  CBackingStore(uint32_t start, uint32_t size);
  ~CBackingStore();
  
  void load(uint32_t dst, uint32_t src_offset, uint32_t size);

private:
  uint32_t iStart_;
  uint32_t iSize_;
};

// -----------------------------------------------------------------------------
// Memory area:
//  - Describes a virtual memory area
class CArea
{
  friend class CAddressSpace;
  
public:
  CArea(uint32_t start, uint32_t size, uint32_t flags);
  ~CArea();
  
  void map(uint32_t vaddr);
  void mapAll();
  
  void start(uint32_t start);
  uint32_t start();
  void size(uint32_t size);
  uint32_t size();
  void flags(uint32_t flags);
  uint32_t flags();
  void backingStore(CBackingStore * bs);
  CBackingStore * backingStore();
  
  bool readable()  {return (iFlags_ & AREA_FLAG_READ);}
  bool writable()  {return (iFlags_ & AREA_FLAG_WRITE);}
  bool executable(){return (iFlags_ & AREA_FLAG_EXECUTE);}
  bool global()    {return (iFlags_ & AREA_FLAG_GLOBAL);}

private:
  uint32_t iStart_;
  uint32_t iSize_;
  uint32_t iFlags_;
  uint32_t iArchFlags_;
  CBackingStore * pBackingStore_;
  
  // Double linked list of area's
  CArea * pAreaNext_;
  CArea * pAreaPrev_;
};

// -----------------------------------------------------------------------------
class CAddressSpace
{
public:
  CAddressSpace();
  ~CAddressSpace();

  void map(CArea * area);
  CArea * findArea(uint32_t vaddr);

  inline uint32_t cr3(){return iCR3_;}

private:
  void areaPushBack(CArea * area);
  
private:
  uint32_t iCR3_;
  
  CArea * pAreaFirst_;
  CArea * pAreaLast_;
};


#endif

