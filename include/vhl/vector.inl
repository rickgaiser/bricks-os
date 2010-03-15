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


#include "vector.h"
#include "math.h"


//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3()
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3(const T * vec)
 : x(vec[0]), y(vec[1]), z(vec[2])
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3(const TVector3 & vec)
 : x(vec.x), y(vec.y), z(vec.z)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3(const TVector4<T> & vec)
 : x(vec.x * vec.w), y(vec.y * vec.w), z(vec.z * vec.w)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3(T _x, T _y, T _z)
 : x(_x), y(_y), z(_z)
{
}

//---------------------------------------------------------------------------
template <class T>
inline T &
TVector3<T>::operator[](uint8_t item)
{
  static T * vector[3] = {&x, &y, &z};

  return *vector[item];
}

//---------------------------------------------------------------------------
template <class T>
inline const T &
TVector3<T>::operator[](uint8_t item) const
{
  static T * vector[3] = {&x, &y, &z};

  return *vector[item];
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator= (const T * vec)
{
  x = vec[0];
  y = vec[1];
  z = vec[2];

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator= (const TVector3 & vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator= (const TVector4<T> & vec)
{
  x = vec.x * vec.w;
  y = vec.y * vec.w;
  z = vec.z * vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator+(const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv += vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator-(const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv -= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator*(T s) const
{
  TVector3 rv(*this);

  rv *= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator/(T s) const
{
  TVector3 rv(*this);

  rv /= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::getInverted() const
{
  TVector3 rv(*this);

  rv.invert();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::getNormalized() const
{
  TVector3 rv(*this);

  rv.normalize();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::getCrossProduct(const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv.crossProduct(vec);

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::getReflection(const TVector3 & normal) const
{
  TVector3 rv(*this);

  rv.reflect(normal);

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
T
TVector3<T>::getLength() const
{
  return sqrtf((x*x) + (y*y) + (z*z));
}

//---------------------------------------------------------------------------
// Scalar product or Dot product
template <class T>
T
TVector3<T>::getDotProduct(const TVector3<T> & vec) const
{
  return ((x*vec.x) + (y*vec.y) + (z*vec.z));
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator+=(const TVector3 & vec)
{
  x += vec.x;
  y += vec.y;
  z += vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator-=(const TVector3 & vec)
{
  x -= vec.x;
  y -= vec.y;
  z -= vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator*=(const T s)
{
  x *= s;
  y *= s;
  z *= s;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator/=(const T s)
{
  x /= s;
  y /= s;
  z /= s;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::invert()
{
  x = 0 - x;
  y = 0 - y;
  z = 0 - z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::normalize()
{
  return ::normalize(*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::crossProduct(const TVector3 & vec)
{
  T tx, ty;

  tx = ((y * vec.z) - (z * vec.y));
  ty = ((z * vec.x) - (x * vec.z));
  z  = ((x * vec.y) - (y * vec.x));
  x  = tx;
  y  = ty;

  return (*this);
}

//---------------------------------------------------------------------------
// Reflect this vector around normal
template <class T>
inline TVector3<T> &
TVector3<T>::reflect(const TVector3 & normal)
{
  *this = normal * 2 * (normal.getDotProduct(*this)) - *this;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
T
TVector3<T>::length() const
{
  return getLength();
}

//---------------------------------------------------------------------------
template <class T>
T
TVector3<T>::dot(const TVector3<T> & vec) const
{
  return getDotProduct(vec);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
normalize(TVector3<T> & vec)
{
  T inv_length = 1 / vec.getLength();

  vec.x *= inv_length;
  vec.y *= inv_length;
  vec.z *= inv_length;

  return vec;
}

//---------------------------------------------------------------------------
// Special case for TFixed class
template <int p>
inline TVector3< TFixed<p> > &
normalize(TVector3< TFixed<p> > & vec)
{
  TFixed<p> length = vec.getLength();

  vec.x /= length;
  vec.y /= length;
  vec.z /= length;

  return vec;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4()
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4(const T * vec)
 : x(vec[0]), y(vec[1]), z(vec[2]), w(vec[3])
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4(const TVector3<T> & vec)
 : x(vec.x), y(vec.y), z(vec.z), w(1)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4(const TVector4 & vec)
 : x(vec.x), y(vec.y), z(vec.z), w(vec.w)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4(T _x, T _y, T _z, T _w)
 : x(_x), y(_y), z(_z), w(_w)
{
}

//---------------------------------------------------------------------------
template <class T>
inline T &
TVector4<T>::operator[](uint8_t item)
{
  static T * vector[4] = {&x, &y, &z, &w};

  return *vector[item];
}

//---------------------------------------------------------------------------
template <class T>
inline const T &
TVector4<T>::operator[](uint8_t item) const
{
  const static T * vector[4] = {&x, &y, &z, &w};

  return *vector[item];
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator= (const T * vec)
{
  x = vec[0];
  y = vec[1];
  z = vec[2];
  w = vec[3];

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator= (const TVector3<T> & vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = 1;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator= (const TVector4 & vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator+(const TVector4 & vec) const
{
  TVector4 rv(*this);

  rv += vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator-(const TVector4 & vec) const
{
  TVector4 rv(*this);

  rv -= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator*(T s) const
{
  TVector4 rv(*this);

  rv *= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator/(T s) const
{
  TVector4 rv(*this);

  rv /= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::getInverted() const
{
  TVector4 rv(*this);

  rv.invert();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::getNormalized() const
{
  TVector4 rv(*this);

  rv.normalize();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline T
TVector4<T>::getLength() const
{
  return (w * sqrtf((x*x) + (y*y) + (z*z)));
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator+=(const TVector4 & vec)
{
#if 0
  // Don't change w
  x += (vec.x * vec.w) / w;
  y += (vec.y * vec.w) / w;
  z += (vec.z * vec.w) / w;
#else
  // Normalize w
  x = (x * w) + (vec.x * vec.w);
  y = (y * w) + (vec.y * vec.w);
  z = (z * w) + (vec.z * vec.w);
  w = 1;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator-=(const TVector4 & vec)
{
#if 0
  // Don't change w
  x -= (vec.x * vec.w) / w;
  y -= (vec.y * vec.w) / w;
  z -= (vec.z * vec.w) / w;
#else
  // Normalize w
  x = (x * w) - (vec.x * vec.w);
  y = (y * w) - (vec.y * vec.w);
  z = (z * w) - (vec.z * vec.w);
  w = 1;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator*=(const T s)
{
#if 0
  // Don't change w
  x *= s;
  y *= s;
  z *= s;
#else
  // Scale w
  w *= s;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator/=(const T s)
{
#if 0
  // Don't change w
  x /= s;
  y /= s;
  z /= s;
#else
  // Scale w
  w /= s;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::invert()
{
#if 0
  // Don't change w
  x = 0 - x;
  y = 0 - y;
  z = 0 - z;
#else
  // Invert w
  w = 0 - w;
#endif

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::normalize()
{
  return ::normalize(*this);
}

//---------------------------------------------------------------------------
template <class T>
inline T
TVector4<T>::length() const
{
  return getLength();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
normalize(TVector4<T> & vec)
{
  T inv_length = 1 / vec.getLength();

  vec.x = vec.x * vec.w * inv_length;
  vec.y = vec.y * vec.w * inv_length;
  vec.z = vec.z * vec.w * inv_length;
  vec.w = 1;

  return vec;
}

//---------------------------------------------------------------------------
// Special case for TFixed class
template <int p>
inline TVector4< TFixed<p> > &
normalize(TVector4< TFixed<p> > & vec)
{
  TFixed<p> length = vec.getLength();

  vec.x = (vec.x * vec.w) / length;
  vec.y = (vec.y * vec.w) / length;
  vec.z = (vec.z * vec.w) / length;
  vec.w = 1;
  
  return vec;
}
