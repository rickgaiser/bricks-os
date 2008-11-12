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


#include "color.h"


//---------------------------------------------------------------------------
template <class T>
inline
TColor<T>::TColor(const T * c)
 : r(c[0]), g(c[1]), b(c[2]), a(c[3])
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TColor<T>::TColor(const TColor<T> & c)
 : r(c.r), g(c.g), b(c.b), a(c.a)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TColor<T>::TColor(T _r, T _g, T _b, T _a)
 : r(_r), g(_g), b(_b), a(_a)
{
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator= (const T * c)
{
  r = c[0];
  g = c[1];
  b = c[2];
  a = c[3];

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator= (const TColor & c)
{
  r = c.r;
  g = c.g;
  b = c.b;
  a = c.a;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::operator+(const TColor & c) const
{
  TColor rv(*this);

  rv += c;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::operator-(const TColor & c) const
{
  TColor rv(*this);

  rv -= c;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::operator*(const TColor & c) const
{
  TColor rv(*this);

  rv *= c;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::operator/(const TColor & c) const
{
  TColor rv(*this);

  rv /= c;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::operator+(T s) const
{
  TColor rv(*this);

  rv += s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::operator-(T s) const
{
  TColor rv(*this);

  rv -= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::operator*(T s) const
{
  TColor rv(*this);

  rv *= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::operator/(T s) const
{
  TColor rv(*this);

  rv /= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T>
TColor<T>::getClamped() const
{
  TColor rv(*this);

  rv.clamp();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator+=(const TColor & c)
{
  r += c.r;
  g += c.g;
  b += c.b;
  a += c.a;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator-=(const TColor & c)
{
  r -= c.r;
  g -= c.g;
  b -= c.b;
  a -= c.a;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator*=(const TColor & c)
{
  r *= c.r;
  g *= c.g;
  b *= c.b;
  a *= c.a;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator/=(const TColor & c)
{
  r /= c.r;
  g /= c.g;
  b /= c.b;
  a /= c.a;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator+=(const T s)
{
  r += s;
  g += s;
  b += s;
  a += s;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator-=(const T s)
{
  r -= s;
  g -= s;
  b -= s;
  a -= s;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator*=(const T s)
{
  r *= s;
  g *= s;
  b *= s;
  a *= s;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::operator/=(const T s)
{
  if(s != 0)
  {
    T s2 = 1.0f / s;

    r *= s2;
    g *= s2;
    b *= s2;
    a *= s2;
  }

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TColor<T> &
TColor<T>::clamp()
{
  if(r < 0) r = 0;
  if(g < 0) g = 0;
  if(b < 0) b = 0;
  if(a < 0) a = 0;

  if(r > 1) r = 1;
  if(g > 1) g = 1;
  if(b > 1) b = 1;
  if(a > 1) a = 1;

  return (*this);
}
