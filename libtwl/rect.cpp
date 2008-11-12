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


#include "rect.h"


namespace twl
{


//---------------------------------------------------------------------------
bool
CRect::valid()
{
  return ((iWidth_ > 0) && (iHeight_ > 0));
}

//---------------------------------------------------------------------------
bool
CRect::contains(const CRect & rect)
{
  return (contains(rect.x(), rect.y()) && contains(rect.right() - 1, rect.bottom() - 1));
}

//---------------------------------------------------------------------------
bool
CRect::contains(int x, int y)
{
  return ((x >= iX_) && (x < iX_ + iWidth_) && (y >= iY_) && (y < iY_ + iHeight_));
}

//---------------------------------------------------------------------------
bool
CRect::intersects(const CRect & rect)
{
  return (contains(rect.x(), rect.y()) || contains(rect.right() - 1, rect.bottom() - 1));
}

//---------------------------------------------------------------------------
void
CRect::clip(const CRect & rect)
{
//  if(intersects(rect) == false)
//  {
    // Invalidate rect
//    *this = CRect(0, 0, 0, 0);
//  }
//  else
  {
    // Clip left
    if(iX_ < rect.iX_)
    {
      iWidth_ -= (rect.iX_ - iX_);
      iX_ = rect.iX_;
    }
    // Clip top
    if(iY_ < rect.iY_)
    {
      iHeight_ -= (rect.iY_ - iY_);
      iY_ = rect.iY_;
    }
    // Clip right
    if((iX_ + iWidth_) > rect.right())
      iWidth_ = rect.right() - iX_;
    // Clip bottom
    if((iY_ + iHeight_) > rect.bottom())
      iHeight_ = rect.bottom() - iY_;
  }
}


};
