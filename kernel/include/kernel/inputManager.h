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


#ifndef KERNEL_INPUT_H
#define KERNEL_INPUT_H


#include "inttypes.h"


enum EInputDeviceType
{
  IDT_KEYBOARD   = 1,
  IDT_MOUSE      = 2,
  IDT_CONTROLLER = 3,
};


// Controller button definitions
#define CTRL_BTN_DIRECTIONAL_MASK 0x0000000f
#define CTRL_BTN_UP               0x00000001
#define CTRL_BTN_DOWN             0x00000002
#define CTRL_BTN_LEFT             0x00000004
#define CTRL_BTN_RIGHT            0x00000008

#define CTRL_BTN_SYSTEM_MASK      0x000000f0
#define CTRL_BTN_START            0x00000010
#define CTRL_BTN_SELECT           0x00000020
#define CTRL_BTN_HOME             0x00000040

#define CTRL_BTN_TRIGGER_MASK     0x00000f00
#define CTRL_BTN_TRIGGER_L1       0x00000100
#define CTRL_BTN_TRIGGER_L2       0x00000200
#define CTRL_BTN_TRIGGER_R1       0x00000400
#define CTRL_BTN_TRIGGER_R2       0x00000800

#define CTRL_BTN_ACTION_MASK      0x000ff000
#define CTRL_BTN_ACTION_1         0x00001000 // Right side up    (ps: triangle, gba/nds: x)
#define CTRL_BTN_ACTION_2         0x00002000 // Right side down  (ps: cross,    gba/nds: b), enter/escape
#define CTRL_BTN_ACTION_3         0x00004000 // Right side left  (ps: square,   gba/nds: y)
#define CTRL_BTN_ACTION_4         0x00008000 // Right side right (ps: circle,   gba/nds: a), escape/enter
#define CTRL_BTN_ACTION_5         0x00010000
#define CTRL_BTN_ACTION_6         0x00020000
#define CTRL_BTN_ACTION_7         0x00040000
#define CTRL_BTN_ACTION_8         0x00080000


//---------------------------------------------------------------------------
class CAInputDevice
{
public:
  CAInputDevice();
  virtual ~CAInputDevice();

  virtual EInputDeviceType type() = 0;
};

//---------------------------------------------------------------------------
class CAControllerDevice
 : public CAInputDevice
{
public:
  virtual ~CAControllerDevice(){}

  EInputDeviceType type(){return IDT_CONTROLLER;};

  virtual uint32_t getButtonState() = 0;
};

//---------------------------------------------------------------------------
#define MAX_INPUT_DEVICE_COUNT 4
class CInputManager
{
public:
  friend class CAInputDevice;

  CInputManager();
  virtual ~CInputManager();

  void listDevices(CAInputDevice ** devices[], int * deviceCount);

private:
  void addDevice(CAInputDevice * device);
  void removeDevice(CAInputDevice * device);

  CAInputDevice * devices_[MAX_INPUT_DEVICE_COUNT];
  int iDeviceCount_;
};


extern CInputManager inputManager;


#endif
