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


#include "mouse.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
CI8042Mouse::CI8042Mouse(C8042 & driver)
 : CAFileIOBufferedRead()
 , driver_(driver)
 , iMouseByteNr_(0)
{
}

// -----------------------------------------------------------------------------
CI8042Mouse::~CI8042Mouse()
{
}

// -----------------------------------------------------------------------------
int
CI8042Mouse::init()
{
  int iRetVal(0);

  // Register our callback handler
  if(driver_.registerHandler(1, this) == true)
  {
    uint8_t dummy;

    //Tell the mouse to use default settings
    driver_.writeData(1, 0xf6);
    driver_.readData(1, &dummy);

    //Enable the mouse
    driver_.writeData(1, 0xf4);
    driver_.readData(1, &dummy);
  }
  else
    iRetVal = -1;

  return iRetVal;
}

// -----------------------------------------------------------------------------
void
CI8042Mouse::i8042_callBack(uint8_t data)
{
  iMouseData_[iMouseByteNr_] = data;
  iMouseByteNr_++;

  if(iMouseByteNr_ == 3)
  {
    iMouseByteNr_ = 0;
    //printk("Mouse keys=0x%x, x=%d, y=%d\n", sMouseData_.keys, sMouseData_.x, sMouseData_.y);
  }
}
