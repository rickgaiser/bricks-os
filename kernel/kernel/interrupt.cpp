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


#include "kernel/interrupt.h"


// -----------------------------------------------------------------------------
CInterrupt::CInterrupt()
{
  // Clear all observer fields
  for(int i(0); i != MAX_OBSERVERS; i++)
  {
    observers_[i] = 0;
  }
}

// -----------------------------------------------------------------------------
CInterrupt::~CInterrupt()
{
}

// -----------------------------------------------------------------------------
void
CInterrupt::attach(IInterruptServiceRoutine * isr)
{
  // Find an empty observer slot and put new isr in it
  for(int i(0); i != MAX_OBSERVERS; i++)
  {
    if(observers_[i] == 0)
    {
      // Found an empty slot
      observers_[i] = isr;
      break;
    }
  }
}

// -----------------------------------------------------------------------------
void
CInterrupt::detach(IInterruptServiceRoutine * isr)
{
  // Find isr slot and delete it
  for(int i(0); i != MAX_OBSERVERS; i++)
  {
    if(observers_[i] == isr)
    {
      // Found isr slot, delete it
      observers_[i] = 0;
    }
  }
}

// -----------------------------------------------------------------------------
int
CInterrupt::isrCount()
{
  int iRetVal(0);

  // Count observers
  for(int i(0); i != MAX_OBSERVERS; i++)
    if(observers_[i] != 0)
      iRetVal++;

  return(iRetVal);
}

// -----------------------------------------------------------------------------
int
CInterrupt::handle(int irq)
{
  // Give all observers a chance to handle the interrupt
  for(int i(0); i != MAX_OBSERVERS; i++)
  {
    if(observers_[i] != 0)
    {
      // Found observer, let him handle the interrupt
      observers_[i]->isr(irq);
    }
  }

  return(0);
}
