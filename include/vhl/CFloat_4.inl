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


#include "CFloat_4.h"


#ifdef __SSE__
  //#define CFLOAT_4_USE_SSE
#endif


//---------------------------------------------------------------------------
inline
CFloat_4::CFloat_4()
{
}

//---------------------------------------------------------------------------
inline
CFloat_4::CFloat_4(const float * v)
{
#ifdef CFLOAT_4_USE_SSE
  xmm = _mm_load_ps(v);
#else
  for(int i = 0; i < 4; i++)
    e[i] = v[i];
#endif // CFLOAT_4_USE_SSE
}

//---------------------------------------------------------------------------
inline
CFloat_4::CFloat_4(const CFloat_4 & v)
{
#ifdef CFLOAT_4_USE_SSE
  xmm = v.xmm;
#else
  for(int i = 0; i < 4; i++)
    e[i] = v.e[i];
#endif // CFLOAT_4_USE_SSE
}

//---------------------------------------------------------------------------
inline
CFloat_4::CFloat_4(const float & e0
                 , const float & e1
                 , const float & e2
                 , const float & e3)
{
#ifdef CFLOAT_4_USE_SSE
  //xmm = _mm_set_ps(e0, e1, e2, e3);
  xmm = _mm_set_ps(e3, e2, e1, e0);
#else
  e[0] = e0;
  e[1] = e1;
  e[2] = e2;
  e[3] = e3;
#endif // CFLOAT_4_USE_SSE
}

//---------------------------------------------------------------------------
inline
CFloat_4::CFloat_4(const float & s)
{
#ifdef CFLOAT_4_USE_SSE
  xmm = _mm_set1_ps(s);
#else
  for(int i = 0; i < 4; i++)
    e[i] = s;
#endif // CFLOAT_4_USE_SSE
}

//---------------------------------------------------------------------------
inline CFloat_4 &
CFloat_4::operator+=(const CFloat_4 & c)
{
#ifdef CFLOAT_4_USE_SSE
  xmm = _mm_add_ps(xmm, c.xmm);
#else
  for(int i = 0; i < 4; i++)
    e[i] += c.e[i];
#endif // CFLOAT_4_USE_SSE

  return (*this);
}

//---------------------------------------------------------------------------
inline CFloat_4 &
CFloat_4::operator-=(const CFloat_4 & c)
{
#ifdef CFLOAT_4_USE_SSE
  xmm = _mm_sub_ps(xmm, c.xmm);
#else
  for(int i = 0; i < 4; i++)
    e[i] -= c.e[i];
#endif // CFLOAT_4_USE_SSE

  return (*this);
}

//---------------------------------------------------------------------------
inline CFloat_4 &
CFloat_4::operator*=(const CFloat_4 & c)
{
#ifdef CFLOAT_4_USE_SSE
  xmm = _mm_mul_ps(xmm, c.xmm);
#else
  for(int i = 0; i < 4; i++)
    e[i] *= c.e[i];
#endif // CFLOAT_4_USE_SSE

  return (*this);
}

//---------------------------------------------------------------------------
inline CFloat_4 &
CFloat_4::operator/=(const CFloat_4 & c)
{
#ifdef CFLOAT_4_USE_SSE
  xmm = _mm_div_ps(xmm, c.xmm);
#else
  for(int i = 0; i < 4; i++)
    e[i] /= c.e[i];
#endif // CFLOAT_4_USE_SSE

  return (*this);
}
