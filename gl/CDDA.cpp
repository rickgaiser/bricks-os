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


#include "CDDA.h"
#include "rasterCommon.h"


const int32_t fxPixelCenterOffset(1 << (SHIFT_XY - 1)); // 0.5
const int32_t fxOneMinusPixelCenterOffset(1 << (SHIFT_XY - 1)); // 1 - 0.5
const int32_t fxPixelFloorOffset(fxPixelCenterOffset - 1); // 0.499...


//-----------------------------------------------------------------------------
inline void
FloorDivMod(int32_t Numerator, int32_t Denominator, int32_t &Floor, int32_t &Mod)
{
  if(Numerator >= 0)
  {
    // positive case, C is okay
    Floor = Numerator / Denominator;
    Mod   = Numerator % Denominator;
  }
  else
  {
    // Numerator is negative, do the right thing
    Floor = -((-Numerator) / Denominator);
    Mod   =   (-Numerator) % Denominator;
    if(Mod)
    {
      // there is a remainder
      Floor--; Mod = Denominator - Mod;
    }
  }
}

//-----------------------------------------------------------------------------
void
CDDA::init(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
  int32_t delta_x = x2 - x1;
  int32_t delta_y = y2 - y1;

  int32_t initial_numerator = 0;
  // x = x1
  initial_numerator += (delta_y * x1);
  // Move down to pixel center
  initial_numerator += (delta_x * ((fpfloor(SHIFT_XY, y1 + fxPixelFloorOffset) << SHIFT_XY) + fxPixelCenterOffset - y1));
  // Add 0.5 or 1 to x
  initial_numerator += (delta_y * fxOneMinusPixelCenterOffset);
  // x        = x1, moved down to the pixel center
  // dxdy_err = horizontal distance from x to the pixel center
  FloorDivMod(initial_numerator,     (delta_y << SHIFT_XY), x_,    dxdy_err_);

  // dxdy     = delta_x / delta_y
  // dxdy_mod = delta_x % delta_y
  FloorDivMod((delta_x << SHIFT_XY), (delta_y << SHIFT_XY), dxdy_, dxdy_mod_);
  // dxdy_den = the value we just used to devide
  dxdy_den_ = delta_y << SHIFT_XY;
}
