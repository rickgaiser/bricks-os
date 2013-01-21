/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2013 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef GL_CDDA_H
#define GL_CDDA_H


#include "inttypes.h"


//-----------------------------------------------------------------------------
class CDDA
{
public:
  CDDA() : x_(0), dxdy_(0), dxdy_mod_(0), dxdy_den_(0), dxdy_err_(0) {}

  void init(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

  inline bool advance(){
    x_ += dxdy_;
    dxdy_err_ += dxdy_mod_;
    if(dxdy_err_ >= dxdy_den_)
    {
      dxdy_err_ -= dxdy_den_;
      x_++;
      return true;
    }
    return false;
  }

  inline int32_t x(){ return x_; }
  inline int32_t dxdy(){ return dxdy_; }

private:
  int32_t x_;
  int32_t dxdy_;
  int32_t dxdy_mod_;
  int32_t dxdy_den_;
  int32_t dxdy_err_;
};


#endif // GL_CDDA_H
