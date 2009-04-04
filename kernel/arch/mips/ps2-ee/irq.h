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


#ifndef PS2_IRQ_H
#define PS2_IRQ_H


#include "kernel/interruptManager.h"
#include "asm/irq.h"
#include "asm/arch/config.h"
#include "r5900.h"


#define INT_GS            0
#define INT_SBUS          1
#define INT_VBLANK_START  2
#define INT_VBLANK_END    3
#define INT_VIF0          4
#define INT_VIF1          5
#define INT_VU0           6
#define INT_VU1           7
#define INT_IPU           8
#define INT_TIMER0        9
#define INT_TIMER1       10


// -----------------------------------------------------------------------------
class CIRQ
 : public IInterruptProvider
#ifdef CONFIG_KERNEL_MODE
 , public IMIPSInterruptHandler
#endif
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

#ifdef CONFIG_KERNEL_MODE
  // Inherited from IInterruptHandler
  void isr(unsigned int irq, pt_regs * regs);
#endif

  // Inherited from IInterruptProvider
  void enable (unsigned int irq);
  void disable(unsigned int irq);

private:
#ifdef CONFIG_KERNEL_MODE
  uint32_t iINTMask_;
#else
  int32_t handle_[MAX_INTERRUPTS];
#endif
};


#endif
