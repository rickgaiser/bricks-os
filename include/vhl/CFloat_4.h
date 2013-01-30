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


#ifndef CFLOAT_4_H
#define CFLOAT_4_H


#ifdef __i386__
#include <immintrin.h>
#endif


//---------------------------------------------------------------------------
class CFloat_4
{
public:
  CFloat_4();
  CFloat_4(const CFloat_4 & v);
  CFloat_4(const float * v);
  CFloat_4(const float & e0
         , const float & e1
         , const float & e2
         , const float & e3);
  CFloat_4(const float & s);

  CFloat_4 & operator+= (const CFloat_4 & v);
  CFloat_4 & operator-= (const CFloat_4 & v);
  CFloat_4 & operator*= (const CFloat_4 & v);
  CFloat_4 & operator/= (const CFloat_4 & v);

  CFloat_4   operator+  (const CFloat_4 & v) const { return (CFloat_4(*this) += v); }
  CFloat_4   operator-  (const CFloat_4 & v) const { return (CFloat_4(*this) -= v); }
  CFloat_4   operator*  (const CFloat_4 & v) const { return (CFloat_4(*this) *= v); }
  CFloat_4   operator/  (const CFloat_4 & v) const { return (CFloat_4(*this) /= v); }

public:
  union
  {
#if defined(__SSE__)
    __m128 xmm;
#endif
    float e[4];
    struct
    {
      union{float x; float r;};
      union{float y; float g;};
      union{float z; float b;};
      union{float w; float a;};
    };
  };
} __attribute__ ((aligned (16)));


#include "CFloat_4.inl"


#endif // CFLOAT_4_H
