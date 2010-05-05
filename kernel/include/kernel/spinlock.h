/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2010 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef KERNEL_SPINLOCK_H
#define KERNEL_SPINLOCK_H


#include "asm/atomic.h"


#ifdef __cplusplus
extern "C"
{
#endif


// -----------------------------------------------------------------------------
typedef struct 
{
  volatile unsigned int lock;
} spinlock_t;

// -----------------------------------------------------------------------------
static inline void
spinlock_init(spinlock_t * lockvar)
{
  return atomic_set(&lockvar->lock, 0);
}

// -----------------------------------------------------------------------------
static inline int
spinlock_trylock(spinlock_t * lockvar)
{
  return (atomic_swap(&lockvar->lock, 1) == 0);
}

// -----------------------------------------------------------------------------
static inline void
spinlock_lock(spinlock_t * lockvar)
{
  while(spinlock_trylock(&lockvar->lock) != 1);
}

// -----------------------------------------------------------------------------
static inline void
spinlock_unlock(spinlock_t * lockvar)
{
  atomic_swap(&lockvar->lock, 0);
}


#ifdef __cplusplus
}
#endif


#endif

