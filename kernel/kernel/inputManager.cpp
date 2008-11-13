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


#include "kernel/inputManager.h"


CInputManager inputManager;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAInputDevice::CAInputDevice()
{
  inputManager.addDevice(this);
}

//---------------------------------------------------------------------------
CAInputDevice::~CAInputDevice()
{
  inputManager.removeDevice(this);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CInputManager::CInputManager()
 : iDeviceCount_(0)
{
  for(int i(0); i < MAX_INPUT_DEVICE_COUNT; i++)
    devices_[i] = 0;
}

//---------------------------------------------------------------------------
CInputManager::~CInputManager()
{
}

//---------------------------------------------------------------------------
void
CInputManager::listDevices(CAInputDevice ** devices[], int * deviceCount)
{
  *devices = devices_;
  *deviceCount = iDeviceCount_;
}

//---------------------------------------------------------------------------
void
CInputManager::addDevice(CAInputDevice * device)
{
  for(int i(0); i < MAX_INPUT_DEVICE_COUNT; i++)
  {
    if(devices_[i] == 0)
    {
      devices_[i] = device;
      iDeviceCount_++;
      break;
    }
  }
}

//---------------------------------------------------------------------------
void
CInputManager::removeDevice(CAInputDevice * device)
{
  for(int i(0); i < MAX_INPUT_DEVICE_COUNT; i++)
  {
    if(devices_[i] == device)
    {
      devices_[i] = 0;
      iDeviceCount_--;
      break;
    }
  }
}
