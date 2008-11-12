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


#ifndef I8042_MOUSE_H
#define I8042_MOUSE_H


#include "i8042.h"
#include "kernel/fs.h"
#include "inttypes.h"


#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_CENTER 0x04


struct SMouseData
{
  uint8_t keys;
  int8_t  x;
  int8_t  y;
} __attribute__ ((__packed__));


// -----------------------------------------------------------------------------
class CI8042Mouse
 : public CAFileIOBufferedRead
 , public I8042CallBack
{
public:
  CI8042Mouse(C8042 & driver);
  virtual ~CI8042Mouse();

  int init();

  // Inherited from ...
  virtual void i8042_callBack(uint8_t data);

private:
  C8042 & driver_;

  uint8_t iMouseByteNr_;

  union
  {
    SMouseData sMouseData_;
    uint8_t iMouseData_[3];
  };
};



#endif
