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


#ifndef ASM_ATOMIC_H
#define ASM_ATOMIC_H


#include "asm/arch/config.h"


#ifdef __cplusplus
extern "C"
{
#endif


// -----------------------------------------------------------------------------
// Multiprocessor systems need to LOCK the bus
// This prevent another CPU from accessing the atomic at the same time
#ifdef CONFIG_SMP
  #define LOCK_ "lock ; "
#else
  #define LOCK_ ""
#endif

// -----------------------------------------------------------------------------
typedef volatile int atomic_t;

// -----------------------------------------------------------------------------
static inline void
atomic_set(atomic_t * atomic, int i)
{
  atomic = i;
}

// -----------------------------------------------------------------------------
static inline int
atomic_get(atomic_t * atomic)
{
  return atomic;
}

// -----------------------------------------------------------------------------
static inline void
atomic_add(atomic_t * atomic, int i)
{
  __asm__ __volatile__(LOCK_ "addl %1,%0":"=m" (atomic):"ir" (i), "m" (atomic));
}

// -----------------------------------------------------------------------------
static inline void
atomic_sub(atomic_t * atomic, int i)
{
  __asm__ __volatile__(LOCK_ "subl %1,%0":"=m" (atomic):"ir" (i), "m" (atomic));
}

// -----------------------------------------------------------------------------
static inline void
atomic_or(atomic_t * atomic, int i)
{
  __asm__ __volatile__(LOCK_ "orl %1,%0":"=m" (atomic):"ir" (i), "m" (atomic));
}

// -----------------------------------------------------------------------------
static inline void
atomic_and(atomic_t * atomic, int i)
{
  __asm__ __volatile__(LOCK_ "andl %1,%0":"=m" (atomic):"ir" (i), "m" (atomic));
}

// -----------------------------------------------------------------------------
static inline void
atomic_inc(atomic_t * atomic)
{
  __asm__ __volatile__(LOCK_ "incl %0":"=m" (atomic):"m" (atomic));
}

// -----------------------------------------------------------------------------
static inline void
atomic_dec(atomic_t * atomic)
{
  __asm__ __volatile__(LOCK_ "decl %0":"=m" (atomic):"m" (atomic));
}

// -----------------------------------------------------------------------------
static inline int
atomic_swap(atomic_t * atomic, int i)
{
  __asm__ __volatile__("xchgl %0,%1":"=r" (i):"m" (atomic), "0" (i):"memory");

  return i;
}


#ifdef __cplusplus
}
#endif


#endif
