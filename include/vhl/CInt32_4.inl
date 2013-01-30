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


#include "vhl/CInt32_4.h"


#if defined(__SSE4_1__)
  //#define CINT32_4_USE_SSE2
  //#define CINT32_4_USE_SSE4_1
#elif defined(__SSE2__)
  //#define CINT32_4_USE_SSE2
#endif


//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4()
{
}

//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4(const int32_t * v)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_set_epi32(v[0], v[1], v[2], v[3]);
#else
  for(int i = 0; i < 4; i++)
    e[i] = v[i];
#endif // CINT32_4_USE_SSE2
}

//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4(const CInt32_4 & v)
{
#ifdef CINT32_4_USE_SSE2
  xmm = v.xmm;
#else
  for(int i = 0; i < 4; i++)
    e[i] = v.e[i];
#endif // CINT32_4_USE_SSE2
}

//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4(const int32_t & e0
                 , const int32_t & e1
                 , const int32_t & e2
                 , const int32_t & e3)
{
#ifdef CINT32_4_USE_SSE2
  //xmm = _mm_set_epi32(e0, e1, e2, e3);
  xmm = _mm_set_epi32(e3, e2, e1, e0);
#else
  e[0] = e0;
  e[1] = e1;
  e[2] = e2;
  e[3] = e3;
#endif // CINT32_4_USE_SSE2
}

//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4(const int32_t & s)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_set1_epi32(s);
#else
  for(int i = 0; i < 4; i++)
    e[i] = s;
#endif // CINT32_4_USE_SSE2
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator+=(const CInt32_4 & v)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_add_epi32(xmm, v.xmm);
#else
  for(int i = 0; i < 4; i++)
    e[i] += v.e[i];
#endif // CINT32_4_USE_SSE2

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator-=(const CInt32_4 & v)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_sub_epi32(xmm, v.xmm);
#else
  for(int i = 0; i < 4; i++)
    e[i] -= v.e[i];
#endif // CINT32_4_USE_SSE2

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator*=(const CInt32_4 & v)
{
#ifdef CINT32_4_USE_SSE4_1
  xmm = _mm_mullo_epi32(xmm, v.xmm);
#else
  for(int i = 0; i < 4; i++)
    e[i] *= v.e[i];
#endif // CINT32_4_USE_SSE4_1

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator/=(const CInt32_4 & v)
{
  for(int i = 0; i < 4; i++)
    e[i] /= v.e[i];

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator>>=(const int32_t & s)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_srai_epi32(xmm, s);
#else
  for(int i = 0; i < 4; i++)
    e[i] >>= s;
#endif // CINT32_4_USE_SSE2

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator<<=(const int32_t & s)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_slli_epi32(xmm, s);
#else
  for(int i = 0; i < 4; i++)
    e[i] <<= s;
#endif // CINT32_4_USE_SSE2

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator&=(const CInt32_4 & v)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_and_si128(xmm, v.xmm);
#else
  for(int i = 0; i < 4; i++)
    e[i] = e[i] & v.e[i];
#endif // CINT32_4_USE_SSE2

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator==(const CInt32_4 & v) const
{
  CInt32_4 rv;

#ifdef CINT32_4_USE_SSE2
  rv.xmm = _mm_cmpeq_epi32(xmm, v.xmm);
#else
  for(int i = 0; i < 4; i++)
    rv.e[i] = e[i] == v.e[i] ? 0xffffffff : 0x00000000;
#endif // CINT32_4_USE_SSE2

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator<(const CInt32_4 & v) const
{
  CInt32_4 rv;

#ifdef CINT32_4_USE_SSE2
  rv.xmm = _mm_cmplt_epi32(xmm, v.xmm);
#else
  for(int i = 0; i < 4; i++)
    rv.e[i] = e[i] < v.e[i] ? 0xffffffff : 0x00000000;
#endif // CINT32_4_USE_SSE2

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator>(const CInt32_4 & v) const
{
  CInt32_4 rv;

#ifdef CINT32_4_USE_SSE2
  rv.xmm = _mm_cmpgt_epi32(xmm, v.xmm);
#else
  for(int i = 0; i < 4; i++)
    rv.e[i] = e[i] > v.e[i] ? 0xffffffff : 0x00000000;
#endif // CINT32_4_USE_SSE2

  return rv;
}

