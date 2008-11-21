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


#include "pad.h"
#include "registers.h"


// -----------------------------------------------------------------------------
CNGCPad::CNGCPad()
 : CAControllerDevice()
{
}

// -----------------------------------------------------------------------------
CNGCPad::~CNGCPad()
{
}

// -----------------------------------------------------------------------------
int
CNGCPad::init()
{
  return 0;
}

// -----------------------------------------------------------------------------
uint32_t
CNGCPad::getButtonState()
{
  uint32_t iRetVal(0);
  uint32_t datah, datal;

  datah  = REG_SI_CHANNEL0_INBUFH;
  datal  = REG_SI_CHANNEL0_INBUFL;

  if(datah & KEY_UP)    iRetVal |= CTRL_BTN_UP;
  if(datah & KEY_DOWN)  iRetVal |= CTRL_BTN_DOWN;
  if(datah & KEY_LEFT)  iRetVal |= CTRL_BTN_LEFT;
  if(datah & KEY_RIGHT) iRetVal |= CTRL_BTN_RIGHT;

  if(datah & KEY_START) iRetVal |= CTRL_BTN_START;

  if(datah & KEY_A)     iRetVal |= CTRL_BTN_ACTION_2; // enter
  if(datah & KEY_B)     iRetVal |= CTRL_BTN_ACTION_4; // escape
  if(datah & KEY_X)     iRetVal |= CTRL_BTN_ACTION_1;
  if(datah & KEY_Y)     iRetVal |= CTRL_BTN_ACTION_3;

  return iRetVal;
}
