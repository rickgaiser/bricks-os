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


#define m00 matrix[RC00]
#define m01 matrix[RC01]
#define m02 matrix[RC02]
#define m03 matrix[RC03]
#define m10 matrix[RC10]
#define m11 matrix[RC11]
#define m12 matrix[RC12]
#define m13 matrix[RC13]
#define m20 matrix[RC20]
#define m21 matrix[RC21]
#define m22 matrix[RC22]
#define m23 matrix[RC23]
#define m30 matrix[RC30]
#define m31 matrix[RC31]
#define m32 matrix[RC32]
#define m33 matrix[RC33]

#define matrix_copy(mto, mfrom) \
mto[RC00] = mfrom[RC00]; mto[RC01] = mfrom[RC01]; mto[RC02] = mfrom[RC02]; mto[RC03] = mfrom[RC03]; \
mto[RC10] = mfrom[RC10]; mto[RC11] = mfrom[RC11]; mto[RC12] = mfrom[RC12]; mto[RC13] = mfrom[RC13]; \
mto[RC20] = mfrom[RC20]; mto[RC21] = mfrom[RC21]; mto[RC22] = mfrom[RC22]; mto[RC23] = mfrom[RC23]; \
mto[RC30] = mfrom[RC30]; mto[RC31] = mfrom[RC31]; mto[RC32] = mfrom[RC32]; mto[RC33] = mfrom[RC33]
//memcpy(mto, mfrom, sizeof(GLfloat) * 16)
#define matrix_clear(m) \
m[RC00] = 0; m[RC01] = 0; m[RC02] = 0; m[RC03] = 0; \
m[RC10] = 0; m[RC11] = 0; m[RC12] = 0; m[RC13] = 0; \
m[RC20] = 0; m[RC21] = 0; m[RC22] = 0; m[RC23] = 0; \
m[RC30] = 0; m[RC31] = 0; m[RC32] = 0; m[RC33] = 0
//memset(m, 0, sizeof(GLfloat) * 16)
#define matrix_identity(m) \
m[RC00] = 1; m[RC01] = 0; m[RC02] = 0; m[RC03] = 0; \
m[RC10] = 0; m[RC11] = 1; m[RC12] = 0; m[RC13] = 0; \
m[RC20] = 0; m[RC21] = 0; m[RC22] = 1; m[RC23] = 0; \
m[RC30] = 0; m[RC31] = 0; m[RC32] = 0; m[RC33] = 1

#define matrixfx_copy(mto, mfrom) \
matrix_copy(mto, mfrom)
//memcpy(mto, mfrom, sizeof(GLfixed) * 16)
#define matrixfx_clear(m) \
matrix_clear(m)
//memset(m, 0, sizeof(GLfixed) * 16)
#define matrixfx_identity(m) \
m[RC00] = gl_fpfromi(1); m[RC01] = gl_fpfromi(0); m[RC02] = gl_fpfromi(0); m[RC03] = gl_fpfromi(0); \
m[RC10] = gl_fpfromi(0); m[RC11] = gl_fpfromi(1); m[RC12] = gl_fpfromi(0); m[RC13] = gl_fpfromi(0); \
m[RC20] = gl_fpfromi(0); m[RC21] = gl_fpfromi(0); m[RC22] = gl_fpfromi(1); m[RC23] = gl_fpfromi(0); \
m[RC30] = gl_fpfromi(0); m[RC31] = gl_fpfromi(0); m[RC32] = gl_fpfromi(0); m[RC33] = gl_fpfromi(1)


template <class T> bool  TMatrix4x4<T>::bInitialized_(false);
template <class T> T     TMatrix4x4<T>::sinTable_[MATRIX_MATH_TABLE_SIZE];
template <class T> T     TMatrix4x4<T>::cosTable_[MATRIX_MATH_TABLE_SIZE];


//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4()
{
  init();
}

//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4(const TMatrix4x4 & m)
{
  *this = m;
  init();
}

//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4(const T * m)
{
  *this = m;
  init();
}

//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4(T _m00, T _m01, T _m02, T _m03,
                          T _m10, T _m11, T _m12, T _m13,
                          T _m20, T _m21, T _m22, T _m23,
                          T _m30, T _m31, T _m32, T _m33)
{
  m00 = _m00; m01 = _m01; m02 = _m02; m03 = _m03;
  m10 = _m10; m11 = _m11; m12 = _m12; m13 = _m13;
  m20 = _m20; m21 = _m21; m22 = _m22; m23 = _m23;
  m30 = _m30; m31 = _m31; m32 = _m32; m33 = _m33;
  init();
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::init()
{
  static const float indexToAngle = M_PI / ((float)(MATRIX_MATH_TABLE_SIZE >> 1));

  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i(0); i < MATRIX_MATH_TABLE_SIZE; i++)
    {
      float angle = (float)i * indexToAngle;
      sinTable_[i] = sinf(angle);
      cosTable_[i] = cosf(angle);
    }
  }
}

//---------------------------------------------------------------------------
template <class T>
inline TMatrix4x4<T> &
TMatrix4x4<T>::operator=(const TMatrix4x4 & m)
{
  return (*this = m.matrix);
}

//---------------------------------------------------------------------------
template <class T>
inline TMatrix4x4<T> &
TMatrix4x4<T>::operator=(const T * m)
{
  matrix_copy(matrix, m);

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::clear()
{
  matrix_clear(matrix);
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::loadIdentity()
{
  matrix_identity(matrix);
}

//---------------------------------------------------------------------------
template <class T>
inline TMatrix4x4<T> &
TMatrix4x4<T>::operator*=(const TMatrix4x4 & m)
{
  return (*this *= m.matrix);
}

//---------------------------------------------------------------------------
// 64 x operator*
// 48 x operator+
template <class T>
inline TMatrix4x4<T> &
TMatrix4x4<T>::operator*=(const T * m)
{
  T   mtemp[16];
  T * ptemp(mtemp);
  T * pmatrix(matrix);
  int iRow = 4;

  while(iRow--)
  {
    T mx0 = *pmatrix++;
    T mx1 = *pmatrix++;
    T mx2 = *pmatrix++;
    T mx3 = *pmatrix++;
    *ptemp++ = mx0 * m[0*4+0] + mx1 * m[1*4+0] + mx2 * m[2*4+0] + mx3 * m[3*4+0];
    *ptemp++ = mx0 * m[0*4+1] + mx1 * m[1*4+1] + mx2 * m[2*4+1] + mx3 * m[3*4+1];
    *ptemp++ = mx0 * m[0*4+2] + mx1 * m[1*4+2] + mx2 * m[2*4+2] + mx3 * m[3*4+2];
    *ptemp++ = mx0 * m[0*4+3] + mx1 * m[1*4+3] + mx2 * m[2*4+3] + mx3 * m[3*4+3];
  }

  return (*this = mtemp);
}

//---------------------------------------------------------------------------
template <class T>
inline TMatrix4x4<T>
TMatrix4x4<T>::operator*(const TMatrix4x4 & m)
{
  TMatrix4x4 mReturn(*this);

  mReturn *= m;

  return mReturn;
}

//---------------------------------------------------------------------------
template <class T>
inline TMatrix4x4<T>
TMatrix4x4<T>::operator*(const T * m)
{
  TMatrix4x4 mReturn(*this);

  mReturn *= m;

  return mReturn;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::translate(T x, T y, T z)
{
  T m[16];
  matrix_identity(m);
  m[RC03] = x;
  m[RC13] = y;
  m[RC23] = z;
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
  T m[16];
  matrix_identity(m);
  m[RC00] = x;
  m[RC11] = y;
  m[RC22] = z;
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
  static const T angleToIndex = ((float)MATRIX_MATH_TABLE_SIZE) / 360.0f;

  // Get sin and cos from lookup table
  int index = (int)(angle * angleToIndex) & (MATRIX_MATH_TABLE_SIZE-1);
  T s = sinTable_[index];
  T c = cosTable_[index];

  T m[16];

  m[RC00] = x*x*(1-c)+c;
  m[RC01] = x*y*(1-c)-z*s;
  m[RC02] = x*z*(1-c)+y*s;
  m[RC03] = 0;

  m[RC10] = y*x*(1-c)+z*s;
  m[RC11] = y*y*(1-c)+c;
  m[RC12] = y*z*(1-c)-x*s;
  m[RC13] = 0;

  m[RC20] = x*z*(1-c)-y*s;
  m[RC21] = y*z*(1-c)+x*s;
  m[RC22] = z*z*(1-c)+c;
  m[RC23] = 0;

  m[RC30] = 0;
  m[RC31] = 0;
  m[RC32] = 0;
  m[RC33] = 1;

  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatex(T angle)
{
  static const T angleToIndex = ((float)MATRIX_MATH_TABLE_SIZE) / 360.0f;

  // Get sin and cos from lookup table
  int index = (int)(angle * angleToIndex) & (MATRIX_MATH_TABLE_SIZE-1);
  T iSin = sinTable_[index];
  T iCos = cosTable_[index];

  T m[16];
  matrix_identity(m);
  m[RC11] = iCos;
  m[RC12] = 0 - iSin;
  m[RC21] = iSin;
  m[RC22] = iCos;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatey(T angle)
{
  static const T angleToIndex = ((float)MATRIX_MATH_TABLE_SIZE) / 360.0f;

  // Get sin and cos from lookup table
  int index = (int)(angle * angleToIndex) & (MATRIX_MATH_TABLE_SIZE-1);
  T iSin = sinTable_[index];
  T iCos = cosTable_[index];

  T m[16];
  matrix_identity(m);
  m[RC00] = iCos;
  m[RC02] = iSin;
  m[RC20] = 0 - iSin;
  m[RC22] = iCos;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatez(T angle)
{
  static const T angleToIndex = ((float)MATRIX_MATH_TABLE_SIZE) / 360.0f;

  // Get sin and cos from lookup table
  int index = (int)(angle * angleToIndex) & (MATRIX_MATH_TABLE_SIZE-1);
  T iSin = sinTable_[index];
  T iCos = cosTable_[index];

  T m[16];
  matrix_identity(m);
  m[RC00] = iCos;
  m[RC01] = iSin;
  m[RC10] = 0 - iSin;
  m[RC11] = iCos;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::transform3(const T * from, T * to)
{
  T x(from[0]);
  T y(from[1]);
  T z(from[2]);

  to[0] = m00 * x + m01 * y + m02 * z;
  to[1] = m10 * x + m11 * y + m12 * z;
  to[2] = m20 * x + m21 * y + m22 * z;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::transform3(const TVector3<T> & from, TVector3<T> & to)
{
  T x(from.x);
  T y(from.y);
  T z(from.z);

  to.x = m00 * x + m01 * y + m02 * z;
  to.y = m10 * x + m11 * y + m12 * z;
  to.z = m20 * x + m21 * y + m22 * z;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::transform3(const TVector4<T> & from, TVector4<T> & to)
{
  T x(from.x);
  T y(from.y);
  T z(from.z);

  to.x = m00 * x + m01 * y + m02 * z;
  to.y = m10 * x + m11 * y + m12 * z;
  to.z = m20 * x + m21 * y + m22 * z;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::transform4(const T * from, T * to)
{
  T x(from[0]);
  T y(from[1]);
  T z(from[2]);
  T w(from[3]);

  to[0] = m00 * x + m01 * y + m02 * z + m03 * w;
  to[1] = m10 * x + m11 * y + m12 * z + m13 * w;
  to[2] = m20 * x + m21 * y + m22 * z + m23 * w;
  to[3] = m30 * x + m31 * y + m32 * z + m33 * w;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::transform4(const TVector4<T> & from, TVector4<T> & to)
{
  T x(from.x);
  T y(from.y);
  T z(from.z);
  T w(from.w);

  to.x = m00 * x + m01 * y + m02 * z + m03 * w;
  to.y = m10 * x + m11 * y + m12 * z + m13 * w;
  to.z = m20 * x + m21 * y + m22 * z + m23 * w;
  to.w = m30 * x + m31 * y + m32 * z + m33 * w;
}
