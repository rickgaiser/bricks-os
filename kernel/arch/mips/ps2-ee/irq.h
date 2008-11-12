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


//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
#define MIPS_INT_0        0
#define MIPS_INT_1        1
#define MIPS_INT_2        2

//---------------------------------------------------------------------------
class IInterruptHandler
{
public:
  virtual ~IInterruptHandler(){}

  virtual void isr(unsigned int irq, pt_regs * regs) = 0;
};
void initExceptions();
void setInterruptHandler(uint32_t nr, IInterruptHandler & handler);

// -----------------------------------------------------------------------------
class CIRQ
 : public IInterruptHandler
 , public IInterruptProvider
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  // Inherited from IInterruptHandler
  void isr(unsigned int irq, pt_regs * regs);

  // Inherited from IInterruptProvider
  void enable (unsigned int irq);
  void disable(unsigned int irq);

private:
  uint32_t iINTMask_;
};


#endif
