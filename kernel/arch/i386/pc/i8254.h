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


// Class for handling Intel 8254 Programmable Interval Timer (PIT)
// Typically located @ 0x40
#ifndef I8254_H
#define I8254_H


#include "stdint.h"
#include "kernel/interrupt.h"


#define I8254_FREQUENCY 1193182 // ~1.2 MHz
#define I8254_COUNTER0_OFF 0
#define I8254_COUNTER1_OFF 1
#define I8254_COUNTER2_OFF 2
#define I8254_CONTROL_OFF 0


class CI8254
 : public IInterruptServiceRoutine
{
public:
  CI8254(uint16_t baseAddr);
  ~CI8254();

  int init();
  void setTimerFrequency(float hz);

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  uint16_t iBaseAddr_;
};


#endif
