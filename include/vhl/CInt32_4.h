/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2012 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef CINT32_4_H
#define CINT32_4_H


#ifdef __i386__
#include <immintrin.h>
#endif


//---------------------------------------------------------------------------
class CInt32_4
{
public:
  CInt32_4();
  CInt32_4(const CInt32_4 & c);
  CInt32_4(const int32_t * c);
  CInt32_4(int32_t _r, int32_t _g, int32_t _b, int32_t _a);

  CInt32_4 & operator=  (const CInt32_4 & c);
  CInt32_4 & operator=  (const int32_t * c);

  CInt32_4   operator+  (const CInt32_4 & c) const;
  CInt32_4   operator-  (const CInt32_4 & c) const;
  CInt32_4   operator*  (const CInt32_4 & c) const;
  CInt32_4   operator/  (const CInt32_4 & c) const;

  CInt32_4   operator+  (int32_t s) const;
  CInt32_4   operator-  (int32_t s) const;
  CInt32_4   operator*  (int32_t s) const;
  CInt32_4   operator/  (int32_t s) const;
  CInt32_4   operator>> (int32_t s) const;
  CInt32_4   operator<< (int32_t s) const;

  CInt32_4 & operator+= (const CInt32_4 & c);
  CInt32_4 & operator-= (const CInt32_4 & c);
  CInt32_4 & operator*= (const CInt32_4 & c);
  CInt32_4 & operator/= (const CInt32_4 & c);

  CInt32_4 & operator+= (int32_t s);
  CInt32_4 & operator-= (int32_t s);
  CInt32_4 & operator*= (int32_t s);
  CInt32_4 & operator/= (int32_t s);
  CInt32_4 & operator>>=(int32_t s);
  CInt32_4 & operator<<=(int32_t s);

public:
  union
  {
#if defined(__SSE__)
    __m128i xmm;
#endif
#if defined(__MMX__)
    __m64 mm[2];
#endif
    int32_t e[4];
    struct
    {
      union{int32_t x; int32_t r;};
      union{int32_t y; int32_t g;};
      union{int32_t z; int32_t b;};
      union{int32_t w; int32_t a;};
    };
  };
} __attribute__ ((aligned (16)));


#include "CInt32_4.inl"


#endif
