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


// Classes for handling Intel 8250 and compatible UART's
#ifndef ARCH_SERIAL_H
#define ARCH_SERIAL_H


#include "kernel/fs.h"
#include "kernel/interrupt.h"
#include "kernel/ringBuffer.h"
#include "sys/types.h"


// -----------------------------------------------------------------------------
class CI8250
 : public IInterruptServiceRoutine
 , public CAFileIOBufferedRead
{
public:
  CI8250(unsigned int baseAddr);
  virtual ~CI8250();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, bool block = false);

private:
  unsigned int iBaseAddr_;
};

// -----------------------------------------------------------------------------
class CI386Serial
{
public:
  CI386Serial();
  virtual ~CI386Serial();

  virtual int init();

//private:
  CI8250 cCom1_;
  CI8250 cCom2_;
  CI8250 cCom3_;
  CI8250 cCom4_;
};


#endif
