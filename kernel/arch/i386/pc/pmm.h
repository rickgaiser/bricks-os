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

/*
 * pmm.h: Physical Memory Management
 */

#ifndef I386_PMM_H
#define I386_PMM_H


// -----------------------------------------------------------------------------
// Kernel Memory Layout
// NOTE: MM_KERNEL_BASE must match the virtual link addr in the linker script!
// -----------------------------------------------------------------------------
// Maximum section sizes (as reserved in the memory layout)
#define MM_KERNEL_CODE_SIZE          ( 4*1024*1024) // Max  4MiB
#define MM_KERNEL_RESERVED_SIZE      (96*1024*1024) // Max 96MiB
#define MM_KERNEL_HEAP_SIZE          (16*1024*1024) // Max 16MiB
#define MM_KERNEL_STACK_SIZE         ( 4*1024*1024) // Max  4MiB
#define MM_KERNEL_OTHER_PT_SIZE      ( 4*1024*1024) // Max  4MiB
#define MM_KERNEL_CURRENT_PT_SIZE    ( 4*1024*1024) // Max  4MiB
#define MM_KERNEL_TOTAL_SIZE         (MM_KERNEL_CODE_SIZE + MM_KERNEL_RESERVED_SIZE + MM_KERNEL_HEAP_SIZE + MM_KERNEL_STACK_SIZE + MM_KERNEL_OTHER_PT_SIZE + MM_KERNEL_CURRENT_PT_SIZE)
#define MM_KERNEL_PT_COUNT           (MM_KERNEL_TOTAL_SIZE>>22)
// Kernel base address
#define MM_KERNEL_BASE               (0xffffffff - (MM_KERNEL_TOTAL_SIZE - 1))
// Section base and top addresses
#define MM_KERNEL_CODE_BASE          (MM_KERNEL_BASE)
#define MM_KERNEL_CODE_TOP           (MM_KERNEL_CODE_BASE + MM_KERNEL_CODE_SIZE)
#define MM_KERNEL_RESERVED_BASE      (MM_KERNEL_CODE_TOP)
#define MM_KERNEL_RESERVED_TOP       (MM_KERNEL_RESERVED_BASE + MM_KERNEL_RESERVED_SIZE)
#define MM_KERNEL_HEAP_BASE          (MM_KERNEL_RESERVED_TOP)
#define MM_KERNEL_HEAP_TOP           (MM_KERNEL_HEAP_BASE + MM_KERNEL_HEAP_SIZE)
#define MM_KERNEL_STACK_BASE         (MM_KERNEL_HEAP_TOP)
#define MM_KERNEL_STACK_TOP          (MM_KERNEL_STACK_BASE + MM_KERNEL_STACK_SIZE)
#define MM_KERNEL_OTHER_PT_BASE      (MM_KERNEL_STACK_TOP)
#define MM_KERNEL_OTHER_PT_TOP       (MM_KERNEL_OTHER_PT_BASE + MM_KERNEL_OTHER_PT_SIZE)
#define MM_KERNEL_CURRENT_PT_BASE    (MM_KERNEL_OTHER_PT_TOP)
#define MM_KERNEL_CURRENT_PT_TOP     (MM_KERNEL_CURRENT_PT_BASE + MM_KERNEL_CURRENT_PT_SIZE)

// -----------------------------------------------------------------------------
// Physical Memory Layout, as loaded by GRUB
// NOTE: PHYS_KERNEL_BASE must match the load addr in the linker script!
// -----------------------------------------------------------------------------
#define PHYS_STACK_SIZE              (16*1024)
#define PHYS_STACK_PAGE_COUNT        (PHYS_STACK_SIZE>>12)
#define PHYS_KERNEL_BASE             (0x00100000) //  1MiB
#define KERNEL_VIRTUAL_OFFSET        (MM_KERNEL_CODE_BASE - PHYS_KERNEL_BASE)

#define PAGE_SIZE                    (4096)
#define PAGES_IN_PT                  (1024)


#ifndef ASM
#include "inttypes.h"


#define VIRT_TO_PHYS(a) ((uint32_t)(a) - KERNEL_VIRTUAL_OFFSET)
#define PHYS_TO_VIRT(a) ((uint32_t)(a) + KERNEL_VIRTUAL_OFFSET)

#define pPTCurrent ((vuint32_t *)MM_KERNEL_CURRENT_PT_BASE)
#define pPDCurrent ((vuint32_t *)(MM_KERNEL_CURRENT_PT_BASE + (4*1024*1024) - (4*1024)))

#define PAGE_ALIGN_DOWN_4K(addr) ((uint32_t)(addr) & 0xfffff000)
#define PAGE_ALIGN_DOWN_2M(addr) ((uint32_t)(addr) & 0xffe00000)
#define PAGE_ALIGN_DOWN_4M(addr) ((uint32_t)(addr) & 0xffc00000)

#define PAGE_ALIGN_UP_4K(addr)   (((uint32_t)(addr) & 0x00000fff) ? (PAGE_ALIGN_DOWN_4K(addr) + 0x00001000) : ((uint32_t)(addr)))
#define PAGE_ALIGN_UP_2M(addr)   (((uint32_t)(addr) & 0x001fffff) ? (PAGE_ALIGN_DOWN_2M(addr) + 0x00200000) : ((uint32_t)(addr)))
#define PAGE_ALIGN_UP_4M(addr)   (((uint32_t)(addr) & 0x003fffff) ? (PAGE_ALIGN_DOWN_4M(addr) + 0x00400000) : ((uint32_t)(addr)))


typedef uint32_t phys_page_index_t;
typedef uint32_t phys_page_addr_t;
typedef uint32_t phys_mem_size_t;


void init_pmm(uint8_t * addr, unsigned int pageCount);

phys_page_addr_t physAllocPageUpper();  // (>= 1024 KiB)
phys_page_addr_t physAllocPageLower();  // (<  1024 KiB)
phys_page_addr_t physAllocPage();

phys_page_addr_t physAllocSizeUpper(phys_mem_size_t length);
phys_page_addr_t physAllocSizeLower(phys_mem_size_t length);
phys_page_addr_t physAllocSize(phys_mem_size_t length);

phys_page_addr_t physAllocRange(phys_page_addr_t addr, phys_mem_size_t length);

void physFreePage(phys_page_addr_t addr);
void physFreeRange(phys_page_addr_t addr, phys_mem_size_t length);

unsigned int usedPageCount();
unsigned int freePageCount();
unsigned int pageCount();
#endif


#endif

