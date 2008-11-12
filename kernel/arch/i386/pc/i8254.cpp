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


#include "i8254.h"
#include "kernel/interruptManager.h"
#include "asm/hal.h"


const float maxTimerFreq(I8254_FREQUENCY);


// -----------------------------------------------------------------------------
CI8254::CI8254(uint16_t baseAddr)
 : iBaseAddr_(baseAddr)
{
}

// -----------------------------------------------------------------------------
CI8254::~CI8254()
{
}

// -----------------------------------------------------------------------------
int
CI8254::init()
{
  // We don't actually "get" the interrupt, but this does enable the it.
  CInterruptManager::attach(0x20, this); // Timer

  return 0;
}

// -----------------------------------------------------------------------------
void
CI8254::setTimerFrequency(float hz)
{
  uint16_t iCounterTimeout = (maxTimerFreq / hz) + 0.5f;

  outb((iCounterTimeout     ) & 0x00ff, iBaseAddr_ + I8254_COUNTER0_OFF);
  outb((iCounterTimeout >> 8) & 0x00ff, iBaseAddr_ + I8254_COUNTER0_OFF);
}

// -----------------------------------------------------------------------------
int
CI8254::isr(int irq)
{
  return 0;
}
