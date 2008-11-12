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


#include "serial.h"
#include "videoDevice.h"
#include "asm/arch/registers.h"


//---------------------------------------------------------------------------
// Real dirty: Wait for line 200 TWICE
#define BUSY_WAIT_VSYNC() \
  while(REG_VI_HLINE != 200); \
  while(REG_VI_HLINE <= 200)

// -----------------------------------------------------------------------------
CNGCSerial::CNGCSerial()
{
}

// -----------------------------------------------------------------------------
CNGCSerial::~CNGCSerial()
{
}

// -----------------------------------------------------------------------------
int
CNGCSerial::init()
{
  REG_SI_POLL            = 0x00000000;
  REG_SI_STATUS          = 0x80000000;
  REG_SI_POLL            = 0x00f70200;
  REG_SI_STATUS          = 0x80000000;
  REG_SI_IO_BUFFER       = 0x00000000;
  REG_SI_COMCSR          = 0xc0010301;
  REG_SI_STATUS          = 0x00000000;

  (void)REG_SI_COMCSR;
  REG_SI_POLL |= 0xF0; // enable all four controller ports

  REG_SI_CHANNEL0_OUTBUF = 0x00400300;
  REG_SI_CHANNEL1_OUTBUF = 0x00400300;
  REG_SI_CHANNEL2_OUTBUF = 0x00400300;
  REG_SI_CHANNEL3_OUTBUF = 0x00400300;

  BUSY_WAIT_VSYNC();

  REG_SI_POLL            = 0x00000000;
  REG_SI_STATUS          = 0x80000000;
  REG_SI_POLL            = 0x00f70200;
  REG_SI_STATUS          = 0x80000000;
  REG_SI_IO_BUFFER       = 0x00000000;
  REG_SI_COMCSR          = 0xc0010301;
  REG_SI_STATUS          = 0x00000000;

  (void)REG_SI_COMCSR;
  REG_SI_POLL |= 0xF0; // enable all four controller ports

  REG_SI_CHANNEL0_OUTBUF = 0x00400300;
  REG_SI_CHANNEL1_OUTBUF = 0x00400300;
  REG_SI_CHANNEL2_OUTBUF = 0x00400300;
  REG_SI_CHANNEL3_OUTBUF = 0x00400300;

  return 0;
}
