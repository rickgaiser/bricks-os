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


#include "kernel/timeValue.h"


// -----------------------------------------------------------------------------
CTimeValue::CTimeValue()
 : iSec_ (0)
 , iUSec_(0)
{
}

// -----------------------------------------------------------------------------
CTimeValue::CTimeValue(long sec, long usec)
 : iSec_ (sec)
 , iUSec_(usec)
{
}

// -----------------------------------------------------------------------------
void
CTimeValue::set(long sec, long usec)
{
  iSec_  = sec;
  iUSec_ = usec;
}

// -----------------------------------------------------------------------------
long
CTimeValue::msec() const
{
  return(iSec_ * 1000 + iUSec_ / 1000);
}

// -----------------------------------------------------------------------------
void
CTimeValue::msec(long msec)
{
  iSec_  = msec / 1000;
  iUSec_ = (msec - (iSec_ * 1000)) * 1000;
}

// -----------------------------------------------------------------------------
void
CTimeValue::normalize()
{
  if(iUSec_ >= 1000000)
  {
    do
    {
      iSec_++;
      iUSec_ -= 1000000;
    }
    while(iUSec_ >= 1000000);
  }
  else if(iUSec_ <= -1000000)
  {
    do
    {
      iSec_--;
      iUSec_ += 1000000;
    }
    while (iUSec_ <= -1000000);
  }

  if(iSec_ >= 1 && iUSec_ < 0)
  {
    iSec_--;
    iUSec_ += 1000000;
  }
  else if (iSec_ < 0 && iUSec_ > 0)
  {
    iSec_++;
    iUSec_ -= 1000000;
  }
}
