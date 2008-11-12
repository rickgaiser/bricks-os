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
inline
CRect::CRect()
 : iX_     (0)
 , iY_     (0)
 , iWidth_ (1)
 , iHeight_(1)
{
}

//---------------------------------------------------------------------------
inline
CRect::CRect(const CRect & rect)
{
  *this = rect;
}

//---------------------------------------------------------------------------
inline
CRect::CRect(int x, int y, int width, int height)
 : iX_     (x)
 , iY_     (y)
 , iWidth_ (width)
 , iHeight_(height)
{
}

//---------------------------------------------------------------------------
inline
CRect::~CRect()
{
}

//---------------------------------------------------------------------------
inline CRect &
CRect::operator=(const CRect & rect)
{
 iX_      = rect.iX_;
 iY_      = rect.iY_;
 iWidth_  = rect.iWidth_;
 iHeight_ = rect.iHeight_;

 return *this;
}

//---------------------------------------------------------------------------
inline int
CRect::x() const
{
  return iX_;
}

//---------------------------------------------------------------------------
inline int
CRect::y() const
{
  return iY_;
}

//---------------------------------------------------------------------------
inline int
CRect::width() const
{
  return iWidth_;
}

//---------------------------------------------------------------------------
inline int
CRect::height() const
{
  return iHeight_;
}

//---------------------------------------------------------------------------
inline int
CRect::left() const
{
  return this->x();
}

//---------------------------------------------------------------------------
inline int
CRect::top() const
{
  return this->y();
}

//---------------------------------------------------------------------------
inline int
CRect::right() const
{
  return (this->x() + this->width());
}

//---------------------------------------------------------------------------
inline int
CRect::bottom() const
{
  return (this->y() + this->height());
}

//---------------------------------------------------------------------------
inline void
CRect::x(int x)
{
  iX_ = x;
}

//---------------------------------------------------------------------------
inline void
CRect::y(int y)
{
  iY_ = y;
}

//---------------------------------------------------------------------------
inline void
CRect::width(int width)
{
  iWidth_ = width;
}

//---------------------------------------------------------------------------
inline void
CRect::height(int height)
{
  iHeight_ = height;
}

//---------------------------------------------------------------------------
inline void
CRect::rect(int x, int y, int width, int height)
{
  iX_      = x;
  iY_      = y;
  iWidth_  = width;
  iHeight_ = height;
}


};
