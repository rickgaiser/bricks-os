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


#define matrix_copy(mto, mfrom) \
mto[ 0] = mfrom[ 0]; mto[ 1] = mfrom[ 1]; mto[ 2] = mfrom[ 2]; mto[ 3] = mfrom[ 3]; \
mto[ 4] = mfrom[ 4]; mto[ 5] = mfrom[ 5]; mto[ 6] = mfrom[ 6]; mto[ 7] = mfrom[ 7]; \
mto[ 8] = mfrom[ 8]; mto[ 9] = mfrom[ 9]; mto[10] = mfrom[10]; mto[11] = mfrom[11]; \
mto[12] = mfrom[12]; mto[13] = mfrom[13]; mto[14] = mfrom[14]; mto[15] = mfrom[15]
//memcpy(mto, mfrom, sizeof(GLfloat) * 16)
#define matrix_clear(m) \
m[ 0] = 0; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0; \
m[ 4] = 0; m[ 5] = 0; m[ 6] = 0; m[ 7] = 0; \
m[ 8] = 0; m[ 9] = 0; m[10] = 0; m[11] = 0; \
m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 0
//memset(m, 0, sizeof(GLfloat) * 16)
#define matrix_identity(m) \
m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0; \
m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0; \
m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0; \
m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1
/*
#define matrixfx_copy(mto, mfrom) \
matrix_copy(mto, mfrom)
//memcpy(mto, mfrom, sizeof(GLfixed) * 16)
#define matrixfx_clear(m) \
matrix_clear(m)
//memset(m, 0, sizeof(GLfixed) * 16)
#define matrixfx_identity(m) \
m[ 0] = gl_fpfromi(1); m[ 1] = gl_fpfromi(0); m[ 2] = gl_fpfromi(0); m[ 3] = gl_fpfromi(0); \
m[ 4] = gl_fpfromi(0); m[ 5] = gl_fpfromi(1); m[ 6] = gl_fpfromi(0); m[ 7] = gl_fpfromi(0); \
m[ 8] = gl_fpfromi(0); m[ 9] = gl_fpfromi(0); m[10] = gl_fpfromi(1); m[11] = gl_fpfromi(0); \
m[12] = gl_fpfromi(0); m[13] = gl_fpfromi(0); m[14] = gl_fpfromi(0); m[15] = gl_fpfromi(1)
*/

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
  m_[ 0] = _m00; m_[ 1] = _m01; m_[ 2] = _m02; m_[ 3] = _m03;
  m_[ 4] = _m10; m_[ 5] = _m11; m_[ 6] = _m12; m_[ 7] = _m13;
  m_[ 8] = _m20; m_[ 9] = _m21; m_[10] = _m22; m_[11] = _m23;
  m_[12] = _m30; m_[13] = _m31; m_[14] = _m32; m_[15] = _m33;
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
  return (*this = m.m_);
}

//---------------------------------------------------------------------------
template <class T>
inline TMatrix4x4<T> &
TMatrix4x4<T>::operator=(const T * m)
{
  matrix_copy(m_, m);

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::clear()
{
  matrix_clear(m_);
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::loadIdentity()
{
  matrix_identity(m_);
}

//---------------------------------------------------------------------------
template <class T>
inline TMatrix4x4<T> &
TMatrix4x4<T>::operator*=(const TMatrix4x4 & m)
{
  return (*this *= m.m_);
}

//---------------------------------------------------------------------------
// 64 x operator*
// 48 x operator+
template <class T>
inline TMatrix4x4<T> &
TMatrix4x4<T>::operator*=(const T * m)
{
  T   mtemp[16];

#if 0
  for(int iRow = 0; iRow < 4; iRow++)
  {
    for(int iCol = 0; iCol < 4; iCol++)
    {
      mtemp[iRow * 4 + iCol] = 0;

      for(int i = 0; i < 4; i++)
      {
        mtemp[iRow * 4 + iCol] += m_[iRow * 4 + i] * m[i * 4 + iCol];
      }
    }
  }
#else
  T   mx0, mx1, mx2, mx3;
  T * ptemp(mtemp);
  T * pmatrix(m_);

  mx0 = *pmatrix++;
  mx1 = *pmatrix++;
  mx2 = *pmatrix++;
  mx3 = *pmatrix++;
  *ptemp++ = mx0 * m[ 0] + mx1 * m[ 4] + mx2 * m[ 8] + mx3 * m[12];
  *ptemp++ = mx0 * m[ 1] + mx1 * m[ 5] + mx2 * m[ 9] + mx3 * m[13];
  *ptemp++ = mx0 * m[ 2] + mx1 * m[ 6] + mx2 * m[10] + mx3 * m[14];
  *ptemp++ = mx0 * m[ 3] + mx1 * m[ 7] + mx2 * m[11] + mx3 * m[15];
  mx0 = *pmatrix++;
  mx1 = *pmatrix++;
  mx2 = *pmatrix++;
  mx3 = *pmatrix++;
  *ptemp++ = mx0 * m[ 0] + mx1 * m[ 4] + mx2 * m[ 8] + mx3 * m[12];
  *ptemp++ = mx0 * m[ 1] + mx1 * m[ 5] + mx2 * m[ 9] + mx3 * m[13];
  *ptemp++ = mx0 * m[ 2] + mx1 * m[ 6] + mx2 * m[10] + mx3 * m[14];
  *ptemp++ = mx0 * m[ 3] + mx1 * m[ 7] + mx2 * m[11] + mx3 * m[15];
  mx0 = *pmatrix++;
  mx1 = *pmatrix++;
  mx2 = *pmatrix++;
  mx3 = *pmatrix++;
  *ptemp++ = mx0 * m[ 0] + mx1 * m[ 4] + mx2 * m[ 8] + mx3 * m[12];
  *ptemp++ = mx0 * m[ 1] + mx1 * m[ 5] + mx2 * m[ 9] + mx3 * m[13];
  *ptemp++ = mx0 * m[ 2] + mx1 * m[ 6] + mx2 * m[10] + mx3 * m[14];
  *ptemp++ = mx0 * m[ 3] + mx1 * m[ 7] + mx2 * m[11] + mx3 * m[15];
  mx0 = *pmatrix++;
  mx1 = *pmatrix++;
  mx2 = *pmatrix++;
  mx3 = *pmatrix++;
  *ptemp++ = mx0 * m[ 0] + mx1 * m[ 4] + mx2 * m[ 8] + mx3 * m[12];
  *ptemp++ = mx0 * m[ 1] + mx1 * m[ 5] + mx2 * m[ 9] + mx3 * m[13];
  *ptemp++ = mx0 * m[ 2] + mx1 * m[ 6] + mx2 * m[10] + mx3 * m[14];
  *ptemp++ = mx0 * m[ 3] + mx1 * m[ 7] + mx2 * m[11] + mx3 * m[15];
#endif

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
  m[ 3] = x;
  m[ 7] = y;
  m[11] = z;
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
  m[ 0] = x;
  m[ 5] = y;
  m[10] = z;
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

  m[ 0] = x*x*(1-c)+c;
  m[ 1] = x*y*(1-c)-z*s;
  m[ 2] = x*z*(1-c)+y*s;
  m[ 3] = 0;

  m[ 4] = y*x*(1-c)+z*s;
  m[ 5] = y*y*(1-c)+c;
  m[ 6] = y*z*(1-c)-x*s;
  m[ 7] = 0;

  m[ 8] = x*z*(1-c)-y*s;
  m[ 9] = y*z*(1-c)+x*s;
  m[10] = z*z*(1-c)+c;
  m[11] = 0;

  m[12] = 0;
  m[13] = 0;
  m[14] = 0;
  m[15] = 1;

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
  m[ 5] = iCos;
  m[ 6] = 0 - iSin;
  m[ 9] = iSin;
  m[10] = iCos;
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
  m[ 0] = iCos;
  m[ 2] = iSin;
  m[ 8] = 0 - iSin;
  m[10] = iCos;
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
  m[ 0] = iCos;
  m[ 1] = iSin;
  m[ 4] = 0 - iSin;
  m[ 5] = iCos;
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

  to[0] = m_[ 0] * x + m_[ 1] * y + m_[ 2] * z;
  to[1] = m_[ 4] * x + m_[ 5] * y + m_[ 6] * z;
  to[2] = m_[ 8] * x + m_[ 9] * y + m_[10] * z;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::transform3(const TVector3<T> & from, TVector3<T> & to)
{
  T x(from.x);
  T y(from.y);
  T z(from.z);

  to.x = m_[ 0] * x + m_[ 1] * y + m_[ 2] * z;
  to.y = m_[ 4] * x + m_[ 5] * y + m_[ 6] * z;
  to.z = m_[ 8] * x + m_[ 9] * y + m_[10] * z;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::transform3(const TVector4<T> & from, TVector4<T> & to)
{
  T x(from.x);
  T y(from.y);
  T z(from.z);

  to.x = m_[ 0] * x + m_[ 1] * y + m_[ 2] * z;
  to.y = m_[ 4] * x + m_[ 5] * y + m_[ 6] * z;
  to.z = m_[ 8] * x + m_[ 9] * y + m_[10] * z;
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

  to[0] = m_[ 0] * x + m_[ 1] * y + m_[ 2] * z + m_[ 3] * w;
  to[1] = m_[ 4] * x + m_[ 5] * y + m_[ 6] * z + m_[ 7] * w;
  to[2] = m_[ 8] * x + m_[ 9] * y + m_[10] * z + m_[11] * w;
  to[3] = m_[12] * x + m_[13] * y + m_[14] * z + m_[15] * w;
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

  to.x = m_[ 0] * x + m_[ 1] * y + m_[ 2] * z + m_[ 3] * w;
  to.y = m_[ 4] * x + m_[ 5] * y + m_[ 6] * z + m_[ 7] * w;
  to.z = m_[ 8] * x + m_[ 9] * y + m_[10] * z + m_[11] * w;
  to.w = m_[12] * x + m_[13] * y + m_[14] * z + m_[15] * w;
}
