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


#include "kernel/video.h"


// -----------------------------------------------------------------------------
CAVideo::CAVideo(unsigned int iX, unsigned int iY)
 : iX_(iX)
 , iY_(iY)
 , iCurrentX_(0)
 , iCurrentY_(0)
{
}

// -----------------------------------------------------------------------------
CAVideo::~CAVideo()
{
}

// -----------------------------------------------------------------------------
void
CAVideo::cls()
{
  for(unsigned int y(0); y < iY_; y++)
  {
    for(unsigned int x(0); x < iX_; x++)
    {
      put(x, y, ' ');
    }
  }
}

// -----------------------------------------------------------------------------
ssize_t
CAVideo::write(const void * buffer, size_t size, bool block)
{
  ssize_t iRetVal(0);

  for(size_t i(0); i < size; i++)
  {
    this->put(((const char *)buffer)[i]);
    iRetVal++;
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
void
CAVideo::put(char c)
{
  switch(c)
  {
  case '\n':
    iCurrentX_ = 0;
    iCurrentY_++;
    break;
  case '\r':
    iCurrentX_ = 0;
    break;
  default:
    this->put(iCurrentX_, iCurrentY_, c);
    iCurrentX_++;
  }

  // Make sure our line/char point to a real char location
  if(iCurrentX_ >= iX_)
  {
    iCurrentX_ = 0;
    iCurrentY_++;
  }
  if(iCurrentY_ >= iY_)
  {
    this->scrollUp();
    iCurrentY_--;
  }
}

// -----------------------------------------------------------------------------
void
CAVideo::put(const char * s)
{
  for(; *s != 0; s++)
  {
    this->put(*s);
  }
}

// -----------------------------------------------------------------------------
void
CAVideo::scrollUp()
{
  // Copy lines
  for(unsigned int y(0); y != (iY_ - 1); y++)
    for(unsigned int x(0); x != iX_; x++)
      put(x, y, get(x, y + 1));

  // Clear last line
  for(unsigned int x(0); x != iX_; x++)
    put(x, iY_ - 1, ' ');
}
