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


#ifndef CFLOAT_2_H
#define CFLOAT_2_H


//---------------------------------------------------------------------------
class CFloat_2
{
public:
  CFloat_2();
  CFloat_2(const CFloat_2 & v);
  CFloat_2(const float * v);
  CFloat_2(const float & e0
         , const float & e1);
  CFloat_2(const float & s);

  CFloat_2 & operator+= (const CFloat_2 & v);
  CFloat_2 & operator-= (const CFloat_2 & v);
  CFloat_2 & operator*= (const CFloat_2 & v);
  CFloat_2 & operator/= (const CFloat_2 & v);

  CFloat_2   operator+  (const CFloat_2 & v) const { return (CFloat_2(*this) += v); }
  CFloat_2   operator-  (const CFloat_2 & v) const { return (CFloat_2(*this) -= v); }
  CFloat_2   operator*  (const CFloat_2 & v) const { return (CFloat_2(*this) *= v); }
  CFloat_2   operator/  (const CFloat_2 & v) const { return (CFloat_2(*this) /= v); }

public:
  union
  {
    float e[2];
    struct
    {
      float u;
      float v;
    };
  };
} __attribute__ ((aligned (16)));


#include "CFloat_2.inl"


#endif // CFLOAT_2_H
