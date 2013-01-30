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


#include "matrix.h"
#include "math.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MATRIX_MATH_TABLE_BITS (10) // 10bits == 1024 values
#define MATRIX_MATH_TABLE_SIZE 1024 // (1<<MATRIX_MATH_TABLE_BITS) <-- Does not work with GCC 3.2.2


bool  bInitialized_(false);
float sinTable_[MATRIX_MATH_TABLE_SIZE];
float cosTable_[MATRIX_MATH_TABLE_SIZE];
const float indexToAngle = M_PI / ((float)(MATRIX_MATH_TABLE_SIZE >> 1));
const float angleToIndex = ((float)MATRIX_MATH_TABLE_SIZE) / 360.0f;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline void
TMatrix4x4_init_tables()
{
  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i = 0; i < MATRIX_MATH_TABLE_SIZE; i++)
    {
      float angle = (float)i * indexToAngle;
      sinTable_[i] = sinf(angle);
      cosTable_[i] = cosf(angle);
    }
  }
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4()
{
  TMatrix4x4_init_tables();
}

//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4(const TMatrix4x4<T> & m)
{
  TMatrix4x4_init_tables();

  for(int i = 0; i < 16; i++)
    m_[i] = m.m_[i];
}

//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4(const T * m)
{
  TMatrix4x4_init_tables();

  for(int i = 0; i < 16; i++)
    m_[i] = m[i];
}

//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4(T _m00, T _m01, T _m02, T _m03,
                          T _m10, T _m11, T _m12, T _m13,
                          T _m20, T _m21, T _m22, T _m23,
                          T _m30, T _m31, T _m32, T _m33)
{
  TMatrix4x4_init_tables();

  m_[ 0] = _m00; m_[ 1] = _m01; m_[ 2] = _m02; m_[ 3] = _m03;
  m_[ 4] = _m10; m_[ 5] = _m11; m_[ 6] = _m12; m_[ 7] = _m13;
  m_[ 8] = _m20; m_[ 9] = _m21; m_[10] = _m22; m_[11] = _m23;
  m_[12] = _m30; m_[13] = _m31; m_[14] = _m32; m_[15] = _m33;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::clear()
{
  for(int i = 0; i < 16; i++)
    m_[i] = 0.0f;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::loadIdentity()
{
  m_[ 0] = 1.0f; m_[ 1] = 0.0f; m_[ 2] = 0.0f; m_[ 3] = 0.0f;
  m_[ 4] = 0.0f; m_[ 5] = 1.0f; m_[ 6] = 0.0f; m_[ 7] = 0.0f;
  m_[ 8] = 0.0f; m_[ 9] = 0.0f; m_[10] = 1.0f; m_[11] = 0.0f;
  m_[12] = 0.0f; m_[13] = 0.0f; m_[14] = 0.0f; m_[15] = 1.0f;
}

//---------------------------------------------------------------------------
// 64 x operator*
// 48 x operator+
template <class T>
inline TMatrix4x4<T> &
TMatrix4x4<T>::operator*=(const TMatrix4x4<T> & m)
{
  TMatrix4x4<T> mtemp;

  for(int i = 0; i < 16; i += 4)
  {
    for(int j = 0; j < 4; j++)
    {
      mtemp.m_[i + j]  = m_[i    ] * m.m_[j     ];
      mtemp.m_[i + j] += m_[i + 1] * m.m_[j +  4];
      mtemp.m_[i + j] += m_[i + 2] * m.m_[j +  8];
      mtemp.m_[i + j] += m_[i + 3] * m.m_[j + 12];
    }
  }

  return (*this = mtemp);
}

//---------------------------------------------------------------------------
template <class T>
inline TMatrix4x4<T>
TMatrix4x4<T>::operator*(const TMatrix4x4<T> & m) const
{
  return (TMatrix4x4<T>(*this) *= m);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TMatrix4x4<T>::operator*(const TVector4<T> & v) const
{
  TVector4<T> rv;

  T x(v.x);
  T y(v.y);
  T z(v.z);
  T w(v.w);

  rv.x = m_[ 0] * x + m_[ 1] * y + m_[ 2] * z + m_[ 3] * w;
  rv.y = m_[ 4] * x + m_[ 5] * y + m_[ 6] * z + m_[ 7] * w;
  rv.z = m_[ 8] * x + m_[ 9] * y + m_[10] * z + m_[11] * w;
  rv.w = m_[12] * x + m_[13] * y + m_[14] * z + m_[15] * w;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TMatrix4x4<T>::operator*(const TVector3<T> & v) const
{
  TVector3<T> rv;

  T x(v.x);
  T y(v.y);
  T z(v.z);

  rv.x = m_[ 0] * x + m_[ 1] * y + m_[ 2] * z;
  rv.y = m_[ 4] * x + m_[ 5] * y + m_[ 6] * z;
  rv.z = m_[ 8] * x + m_[ 9] * y + m_[10] * z;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::translate(T x, T y, T z)
{
  TMatrix4x4<T> m;

  m.loadIdentity();
  m.m_[ 3] = x;
  m.m_[ 7] = y;
  m.m_[11] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::translate(const T * vec)
{
  translate(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::scale(T x, T y, T z)
{
  TMatrix4x4<T> m;

  m.loadIdentity();
  m.m_[ 0] = x;
  m.m_[ 5] = y;
  m.m_[10] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::scale(const T * vec)
{
  scale(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotate(T angle, T x, T y, T z)
{
  // Get sin and cos from lookup table
  int index = (int)(angle * angleToIndex) & (MATRIX_MATH_TABLE_SIZE-1);
  T s = sinTable_[index];
  T c = cosTable_[index];

  TMatrix4x4<T> m;

  m.m_[ 0] = x*x*(1-c)+c;
  m.m_[ 1] = x*y*(1-c)-z*s;
  m.m_[ 2] = x*z*(1-c)+y*s;
  m.m_[ 3] = 0;

  m.m_[ 4] = y*x*(1-c)+z*s;
  m.m_[ 5] = y*y*(1-c)+c;
  m.m_[ 6] = y*z*(1-c)-x*s;
  m.m_[ 7] = 0;

  m.m_[ 8] = x*z*(1-c)-y*s;
  m.m_[ 9] = y*z*(1-c)+x*s;
  m.m_[10] = z*z*(1-c)+c;
  m.m_[11] = 0;

  m.m_[12] = 0;
  m.m_[13] = 0;
  m.m_[14] = 0;
  m.m_[15] = 1;

  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatex(T angle)
{
  // Get sin and cos from lookup table
  int index = (int)(angle * angleToIndex) & (MATRIX_MATH_TABLE_SIZE-1);
  T iSin = sinTable_[index];
  T iCos = cosTable_[index];

  TMatrix4x4<T> m;

  m.loadIdentity();
  m.m_[ 5] = iCos;
  m.m_[ 6] = 0 - iSin;
  m.m_[ 9] = iSin;
  m.m_[10] = iCos;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatey(T angle)
{
  // Get sin and cos from lookup table
  int index = (int)(angle * angleToIndex) & (MATRIX_MATH_TABLE_SIZE-1);
  T iSin = sinTable_[index];
  T iCos = cosTable_[index];

  TMatrix4x4<T> m;

  m.loadIdentity();
  m.m_[ 0] = iCos;
  m.m_[ 2] = iSin;
  m.m_[ 8] = 0 - iSin;
  m.m_[10] = iCos;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatez(T angle)
{
  // Get sin and cos from lookup table
  int index = (int)(angle * angleToIndex) & (MATRIX_MATH_TABLE_SIZE-1);
  T iSin = sinTable_[index];
  T iCos = cosTable_[index];

  TMatrix4x4<T> m;

  m.loadIdentity();
  m.m_[ 0] = iCos;
  m.m_[ 1] = iSin;
  m.m_[ 4] = 0 - iSin;
  m.m_[ 5] = iCos;
  *this *= m;
}
