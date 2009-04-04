/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef PS2_R5900_H
#define PS2_R5900_H


#include "kernel/interruptManager.h"
#include "asm/irq.h"


// Exceptions
#define MIPS_EXCEPTION_TLB_REFILL 0
#define MIPS_EXCEPTION_COUNTER    1
#define MIPS_EXCEPTION_DEBUG      2
#define MIPS_EXCEPTION_COMMON     3
#define MIPS_EXCEPTION_INTERRUPT  4

// Interrupts Pins (exception MIPS_EXCEPTION_INTERRUPT)
#define MIPS_INTERRUPT_0          0 // Interrupt Controller
#define MIPS_INTERRUPT_1          1 // DMA Controller
#define MIPS_INTERRUPT_2          2 // Not used


// Cache
/**
 * Flush complete data cache.
 * Interrupts need to be disabled before calling this function.
 * Exceptioins are not allowed, memory need to be mapped.
 */
extern "C" void flushDCacheAll(void);

/**
 * Invalidate complete instuction cache.
 * Interrupts need to be disabled before calling this function.
 */
extern "C" void invalidateICacheAll(void);

class IMIPSInterruptHandler
{
public:
  virtual ~IMIPSInterruptHandler(){}

  virtual void isr(unsigned int irq, pt_regs * regs) = 0;
};

// Exceptions
void initExceptions();
// MIPS interrupts
void setInterruptHandler(uint32_t nr, IMIPSInterruptHandler & handler);


#endif
