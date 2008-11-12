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


#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/cpu.h"


// -----------------------------------------------------------------------------
// Static data members
CInterrupt   CInterruptManager::interrupt_[MAX_INTERRUPTS];


// -----------------------------------------------------------------------------
void
CInterruptManager::attach(unsigned int irq, IInterruptServiceRoutine * isr)
{
  if(irq < MAX_INTERRUPTS)
  {
    interrupt_[irq].attach(isr);
    // If this is the first ISR, enable the interrupt
    if(interrupt_[irq].isrCount() == 1)
    {
      interrupt_[irq].hardware_->enable(irq);
    }
  }
}

// -----------------------------------------------------------------------------
void
CInterruptManager::detach(unsigned int irq, IInterruptServiceRoutine * isr)
{
  if(irq < MAX_INTERRUPTS)
  {
    interrupt_[irq].detach(isr);
    // If this is the last ISR, disable the interrupt
    if(interrupt_[irq].isrCount() == 0)
    {
      interrupt_[irq].hardware_->disable(irq);
    }
  }
}

// -----------------------------------------------------------------------------
void
CInterruptManager::attach(unsigned int irq, IInterruptProvider * irqhardware)
{
  if(irq < MAX_INTERRUPTS)
    interrupt_[irq].hardware_ = irqhardware;
}

// -----------------------------------------------------------------------------
void
CInterruptManager::detach(unsigned int irq, IInterruptProvider * irqhardware)
{
  if(irq < MAX_INTERRUPTS)
    interrupt_[irq].hardware_ = 0;
}

// -----------------------------------------------------------------------------
unsigned int
CInterruptManager::isr(unsigned int irq, pt_regs * regs)
{
  if(irq < MAX_INTERRUPTS)
  {
    if(interrupt_[irq].isrCount() != 0)
    {
      interrupt_[irq].handle(irq);
    }
    else
    {
      printk("CInterruptManager::isr: ERROR: Unhandled interrupt(int=%d)\n", (int)irq);
    }
  }
  else
  {
    printk("CInterruptManager::isr: ERROR: Interrupt out of range(int=%d)\n", (int)irq);
  }

  return 0;
}
