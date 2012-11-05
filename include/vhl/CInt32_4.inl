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
  #define CINT32_4_USE_SSE2
  #define CINT32_4_USE_SSE4_1
#elif defined(__SSE2__)
  #define CINT32_4_USE_SSE2
#endif


//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4()
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_setzero_si128();
#else
  r = 0;
  g = 0;
  b = 0;
  a = 0;
#endif
}

//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4(const int32_t * c)
{
  *this = c;
}

//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4(const CInt32_4 & c)
{
  *this = c;
}

//---------------------------------------------------------------------------
inline
CInt32_4::CInt32_4(int32_t _r, int32_t _g, int32_t _b, int32_t _a)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_set_epi32(r, g, b, a);
#else
  r = _r;
  g = _g;
  b = _b;
  a = _a;
#endif
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator= (const int32_t * c)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_set_epi32(c[0], c[1], c[2], c[3]);
#else
  r = c[0];
  g = c[1];
  b = c[2];
  a = c[3];
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator= (const CInt32_4 & c)
{
#ifdef CINT32_4_USE_SSE2
  xmm = c.xmm;
#else
  r = c.r;
  g = c.g;
  b = c.b;
  a = c.a;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator+(const CInt32_4 & c) const
{
  CInt32_4 rv(*this);

  rv += c;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator-(const CInt32_4 & c) const
{
  CInt32_4 rv(*this);

  rv -= c;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator*(const CInt32_4 & c) const
{
  CInt32_4 rv(*this);

  rv *= c;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator/(const CInt32_4 & c) const
{
  CInt32_4 rv(*this);

  rv /= c;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator+(int32_t s) const
{
  CInt32_4 rv(*this);

  rv += s;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator-(int32_t s) const
{
  CInt32_4 rv(*this);

  rv -= s;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator*(int32_t s) const
{
  CInt32_4 rv(*this);

  rv *= s;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator/(int32_t s) const
{
  CInt32_4 rv(*this);

  rv /= s;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator>>(int32_t s) const
{
  CInt32_4 rv(*this);

  rv >>= s;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4
CInt32_4::operator<<(int32_t s) const
{
  CInt32_4 rv(*this);

  rv <<= s;

  return rv;
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator+=(const CInt32_4 & c)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_add_epi32(xmm, c.xmm);
#else
  r += c.r;
  g += c.g;
  b += c.b;
  a += c.a;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator-=(const CInt32_4 & c)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_sub_epi32(xmm, c.xmm);
#else
  r -= c.r;
  g -= c.g;
  b -= c.b;
  a -= c.a;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator*=(const CInt32_4 & c)
{
#ifdef CINT32_4_USE_SSE4_1
  xmm = _mm_mullo_epi32(xmm, c.xmm);
#else
  r *= c.r;
  g *= c.g;
  b *= c.b;
  a *= c.a;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator/=(const CInt32_4 & c)
{
  r /= c.r;
  g /= c.g;
  b /= c.b;
  a /= c.a;

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator+=(const int32_t s)
{
#ifdef CINT32_4_USE_SSE2
  __m128i xmm_tmp = _mm_set_epi32(s, s, s, s);
  xmm = _mm_add_epi32(xmm, xmm_tmp);
#else
  r += s;
  g += s;
  b += s;
  a += s;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator-=(const int32_t s)
{
#ifdef CINT32_4_USE_SSE2
  __m128i xmm_tmp = _mm_set_epi32(s, s, s, s);
  xmm = _mm_sub_epi32(xmm, xmm_tmp);
#else
  r -= s;
  g -= s;
  b -= s;
  a -= s;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator*=(const int32_t s)
{
#ifdef CINT32_4_USE_SSE4_1
  __m128i xmm_tmp = _mm_set_epi32(s, s, s, s);
  xmm = _mm_mullo_epi32(xmm, xmm_tmp);
#else
  r *= s;
  g *= s;
  b *= s;
  a *= s;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator/=(const int32_t s)
{
  r /= s;
  g /= s;
  b /= s;
  a /= s;

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator>>=(const int32_t s)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_srai_epi32(xmm, s);
#else
  r >>= s;
  g >>= s;
  b >>= s;
  a >>= s;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
inline CInt32_4 &
CInt32_4::operator<<=(const int32_t s)
{
#ifdef CINT32_4_USE_SSE2
  xmm = _mm_slli_epi32(xmm, s);
#else
  r <<= s;
  g <<= s;
  b <<= s;
  a <<= s;
#endif

  return (*this);
}
