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


#include "CFloat_2.h"


//---------------------------------------------------------------------------
inline
CFloat_2::CFloat_2()
{
}

//---------------------------------------------------------------------------
inline
CFloat_2::CFloat_2(const float * v)
{
  for(int i = 0; i < 2; i++)
    e[i] = v[i];
}

//---------------------------------------------------------------------------
inline
CFloat_2::CFloat_2(const CFloat_2 & v)
{
  for(int i = 0; i < 2; i++)
    e[i] = v.e[i];
}

//---------------------------------------------------------------------------
inline
CFloat_2::CFloat_2(const float & e0
                 , const float & e1)
{
  e[0] = e0;
  e[1] = e1;
}

//---------------------------------------------------------------------------
inline
CFloat_2::CFloat_2(const float & s)
{
  for(int i = 0; i < 2; i++)
    e[i] = s;
}

//---------------------------------------------------------------------------
inline CFloat_2 &
CFloat_2::operator+=(const CFloat_2 & c)
{
  for(int i = 0; i < 2; i++)
    e[i] += c.e[i];

  return (*this);
}

//---------------------------------------------------------------------------
inline CFloat_2 &
CFloat_2::operator-=(const CFloat_2 & c)
{
  for(int i = 0; i < 2; i++)
    e[i] -= c.e[i];

  return (*this);
}

//---------------------------------------------------------------------------
inline CFloat_2 &
CFloat_2::operator*=(const CFloat_2 & c)
{
  for(int i = 0; i < 2; i++)
    e[i] *= c.e[i];

  return (*this);
}

//---------------------------------------------------------------------------
inline CFloat_2 &
CFloat_2::operator/=(const CFloat_2 & c)
{
  for(int i = 0; i < 2; i++)
    e[i] /= c.e[i];

  return (*this);
}
