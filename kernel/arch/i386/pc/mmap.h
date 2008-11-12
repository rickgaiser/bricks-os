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


#ifndef I386_MMAP_H
#define I386_MMAP_H


#include "inttypes.h"


#define PAGE_ALIGN_DOWN_4K(addr) ((uint32_t)addr & 0xfffff000)
#define PAGE_ALIGN_DOWN_2M(addr) ((uint32_t)addr & 0xffe00000)
#define PAGE_ALIGN_DOWN_4M(addr) ((uint32_t)addr & 0xffc00000)

#define PAGE_ALIGN_UP_4K(addr)   (((uint32_t)addr & 0x00000fff) ? (PAGE_ALIGN_DOWN_4K(addr) + 0x00001000) : ((uint32_t)addr))
#define PAGE_ALIGN_UP_2M(addr)   (((uint32_t)addr & 0x001fffff) ? (PAGE_ALIGN_DOWN_2M(addr) + 0x00200000) : ((uint32_t)addr))
#define PAGE_ALIGN_UP_4M(addr)   (((uint32_t)addr & 0x003fffff) ? (PAGE_ALIGN_DOWN_4M(addr) + 0x00400000) : ((uint32_t)addr))


void     init_mmap(uint8_t * addr, unsigned int pageCount);

uint64_t physAllocPageHigh(); // (>= 1088 KiB)
uint32_t physAllocPageLow();  // (<  1024 KiB)
void     physFreePage(uint64_t addr);
uint64_t physAllocRange(uint64_t start, uint64_t length);
void     physFreeRange(uint64_t start, uint64_t length);

unsigned int usedPageCount();
unsigned int freePageCount();
unsigned int pageCount();


#endif

