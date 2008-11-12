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


#ifndef EXTERNALMEMORY_H
#define EXTERNALMEMORY_H


#include "kernel/queue.h"
#include "inttypes.h"
#include "stddef.h"


//---------------------------------------------------------------------------
struct SExternalMemoryBlock
{
  uint32_t base; // Base addr of memory allocation
  uint32_t size; // Size in bytes of memory allocation
  bool     allocated;
  TAILQ_ENTRY(SExternalMemoryBlock) queueEntry;
};

TAILQ_HEAD(SExternalMemoryBlockQueue, SExternalMemoryBlock);

//---------------------------------------------------------------------------
class CExternalMemory
{
public:
  CExternalMemory(uint32_t base, uint32_t size, uint32_t blockSize);
  ~CExternalMemory();

  void * malloc(uint32_t size);
  void   free(void * base);

private:
  SExternalMemoryBlockQueue queueHead_;
  uint32_t base_;
  uint32_t size_;
  uint32_t blockSize_;
};


#endif
