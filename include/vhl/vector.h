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


#ifndef GL_VECTOR_H
#define GL_VECTOR_H


#include "stdint.h"
#include "fixedPoint.h"


//---------------------------------------------------------------------------
template <class T>
class TVector4;

//---------------------------------------------------------------------------
template <class T>
class TVector3
{
public:
  // Constructors
  TVector3();
  TVector3(const T * vec);
  TVector3(const TVector3 & vec);
  TVector3(const TVector4<T> & vec);
  TVector3(T _x, T _y, T _z);

  // Accessor
  T & operator[](uint8_t item);
  const T & operator[](uint8_t item) const;

  TVector3 & operator= (const T * vec);
  TVector3 & operator= (const TVector3 & vec);
  TVector3 & operator= (const TVector4<T> & vec);

  TVector3   operator+ (const TVector3 & vec) const;
  TVector3   operator- (const TVector3 & vec) const;
  TVector3   operator* (T s) const;
  TVector3   operator/ (T s) const;
  TVector3   getInverted() const;
  TVector3   getNormalized() const;
  TVector3   getCrossProduct(const TVector3 & vec) const;
  TVector3   getReflection(const TVector3 & normal) const;
  T          getLength() const;
  T          getDotProduct(const TVector3 & vec) const;

  TVector3 & operator+=(const TVector3 & vec);
  TVector3 & operator-=(const TVector3 & vec);
  TVector3 & operator*=(T s);
  TVector3 & operator/=(T s);
  TVector3 & invert();
  TVector3 & normalize();
  TVector3 & crossProduct(const TVector3 & vec);
  TVector3 & reflect(const TVector3 & normal);

  // For convenience
  T          length() const; // same as getLength
  T          dot(const TVector3 & vec) const; // same as getDotProduct

public:
  T x;
  T y;
  T z;
};
// Normalize needs an optimized version for TFixed class
template <class T> inline TVector3<T> & normalize(TVector3<T> & vec);
template <int p> inline TVector3< TFixed<p> > & normalize(TVector3< TFixed<p> > & vec);

//---------------------------------------------------------------------------
template <class T>
class TVector4
{
public:
  // Constructors
  TVector4();
  TVector4(const T * vec);
  TVector4(const TVector3<T> & vec);
  TVector4(const TVector4 & vec);
  TVector4(T _x, T _y, T _z, T _w);

  // Accessor
  T & operator[](uint8_t item);
  const T & operator[](uint8_t item) const;

  TVector4 & operator= (const T * vec);
  TVector4 & operator= (const TVector3<T> & vec);
  TVector4 & operator= (const TVector4 & vec);

  TVector4   operator+ (const TVector4 & vec) const;
  TVector4   operator- (const TVector4 & vec) const;
  TVector4   operator* (T s) const;
  TVector4   operator/ (T s) const;
  TVector4   getInverted() const;
  TVector4   getNormalized() const;
  //TVector4   getCrossProduct(const TVector4<T> & vec) const;
  //TVector4   getReflection(const TVector4 & normal) const;
  T          getLength() const;
  //T          getDotProduct(const TVector3<T> & vec) const;

  TVector4 & operator+=(const TVector4 & vec);
  TVector4 & operator-=(const TVector4 & vec);
  TVector4 & operator*=(T s);
  TVector4 & operator/=(T s);
  TVector4 & invert();
  TVector4 & normalize();
  //TVector4 & crossProduct(const TVector4<T> & vec);
  //TVector4 & reflect(const TVector4 & normal);

  // For convenience
  T          length() const; // same as getLength
  //T          dot(const TVector4 & vec) const; // same as getDotProduct

public:
  T x;
  T y;
  T z;
  T w;
};
// Normalize needs an optimized version for TFixed class
template <class T> inline TVector4<T> & normalize(TVector4<T> & vec);
template <int p> inline TVector4< TFixed<p> > & normalize(TVector4< TFixed<p> > & vec);


#include "vector.inl"


#endif // GL_VECTOR_H
