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
#include <pspctrl.h>


// -----------------------------------------------------------------------------
CPSPPad::CPSPPad()
 : CAControllerDevice()
{
}

// -----------------------------------------------------------------------------
CPSPPad::~CPSPPad()
{
}

// -----------------------------------------------------------------------------
int
CPSPPad::init()
{
  return 0;
}

// -----------------------------------------------------------------------------
uint32_t
CPSPPad::getButtonState()
{
  uint32_t iRetVal(0);
  SceCtrlData pad;

  sceCtrlReadBufferPositive(&pad, 1);

  if(pad.Buttons & PSP_CTRL_UP)       iRetVal |= CTRL_BTN_UP;
  if(pad.Buttons & PSP_CTRL_DOWN)     iRetVal |= CTRL_BTN_DOWN;
  if(pad.Buttons & PSP_CTRL_LEFT)     iRetVal |= CTRL_BTN_LEFT;
  if(pad.Buttons & PSP_CTRL_RIGHT)    iRetVal |= CTRL_BTN_RIGHT;

  if(pad.Buttons & PSP_CTRL_START)    iRetVal |= CTRL_BTN_START;
  if(pad.Buttons & PSP_CTRL_SELECT)   iRetVal |= CTRL_BTN_SELECT;
  if(pad.Buttons & PSP_CTRL_HOME)     iRetVal |= CTRL_BTN_HOME;

  if(pad.Buttons & PSP_CTRL_LTRIGGER) iRetVal |= CTRL_BTN_TRIGGER_L1;
  if(pad.Buttons & PSP_CTRL_RTRIGGER) iRetVal |= CTRL_BTN_TRIGGER_R1;

  if(pad.Buttons & PSP_CTRL_TRIANGLE) iRetVal |= CTRL_BTN_ACTION_1;
  if(pad.Buttons & PSP_CTRL_CROSS)    iRetVal |= CTRL_BTN_ACTION_2;
  if(pad.Buttons & PSP_CTRL_SQUARE)   iRetVal |= CTRL_BTN_ACTION_3;
  if(pad.Buttons & PSP_CTRL_CIRCLE)   iRetVal |= CTRL_BTN_ACTION_4;

  return iRetVal;
}
