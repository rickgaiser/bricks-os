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


#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H


#include "interrupt.h"
#include "asm/irq.h"


// Early declaration
struct pt_regs;

//---------------------------------------------------------------------------
class CInterruptManager
{
public:
  static void attach(unsigned int irq, IInterruptServiceRoutine * isr);
  static void detach(unsigned int irq, IInterruptServiceRoutine * isr);

  static void attach(unsigned int irq, IInterruptProvider * irqhardware);
  static void detach(unsigned int irq, IInterruptProvider * irqhardware);

  static unsigned int isr(unsigned int irq, pt_regs * regs);

private:
  CInterruptManager(){}

  static CInterrupt   interrupt_[MAX_INTERRUPTS];  // Interrupt objects
};


#endif
