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


#ifndef INTERRUPT_H
#define INTERRUPT_H


#define MAX_OBSERVERS 2


// -----------------------------------------------------------------------------
class IInterruptProvider
{
public:
  virtual ~IInterruptProvider(){}

  virtual void enable (unsigned int irq) = 0;
  virtual void disable(unsigned int irq) = 0;
};

//---------------------------------------------------------------------------
class IInterruptServiceRoutine
{
public:
  virtual ~IInterruptServiceRoutine(){}

  virtual int isr(int irq) = 0;
};

//---------------------------------------------------------------------------
class CInterrupt
{
public:
  CInterrupt();
  virtual ~CInterrupt();

  virtual void attach(IInterruptServiceRoutine * isr);
  virtual void detach(IInterruptServiceRoutine * isr);

  virtual int  isrCount();

  virtual int  handle(int isr);

  IInterruptProvider * hardware_;

private:
  IInterruptServiceRoutine * observers_[MAX_OBSERVERS];
};


#endif
