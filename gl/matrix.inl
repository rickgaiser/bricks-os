#include "matrix.h"
#include "string.h"
typedef unsigned int wint_t;
#include <math.h>


#define matrix_copy(mto, mfrom) \
mto[0*4+0] = mfrom[0*4+0]; mto[0*4+1] = mfrom[0*4+1]; mto[0*4+2] = mfrom[0*4+2]; mto[0*4+3] = mfrom[0*4+3]; \
mto[1*4+0] = mfrom[1*4+0]; mto[1*4+1] = mfrom[1*4+1]; mto[1*4+2] = mfrom[1*4+2]; mto[1*4+3] = mfrom[1*4+3]; \
mto[2*4+0] = mfrom[2*4+0]; mto[2*4+1] = mfrom[2*4+1]; mto[2*4+2] = mfrom[2*4+2]; mto[2*4+3] = mfrom[2*4+3]; \
mto[3*4+0] = mfrom[3*4+0]; mto[3*4+1] = mfrom[3*4+1]; mto[3*4+2] = mfrom[3*4+2]; mto[3*4+3] = mfrom[3*4+3]
//memcpy(mto, mfrom, sizeof(GLfloat) * 16)
#define matrix_clear(m) \
m[0*4+0] = 0; m[0*4+1] = 0; m[0*4+2] = 0; m[0*4+3] = 0; \
m[1*4+0] = 0; m[1*4+1] = 0; m[1*4+2] = 0; m[1*4+3] = 0; \
m[2*4+0] = 0; m[2*4+1] = 0; m[2*4+2] = 0; m[2*4+3] = 0; \
m[3*4+0] = 0; m[3*4+1] = 0; m[3*4+2] = 0; m[3*4+3] = 0
//memset(m, 0, sizeof(GLfloat) * 16)
#define matrix_identity(m) \
m[0*4+0] = 1; m[0*4+1] = 0; m[0*4+2] = 0; m[0*4+3] = 0; \
m[1*4+0] = 0; m[1*4+1] = 1; m[1*4+2] = 0; m[1*4+3] = 0; \
m[2*4+0] = 0; m[2*4+1] = 0; m[2*4+2] = 1; m[2*4+3] = 0; \
m[3*4+0] = 0; m[3*4+1] = 0; m[3*4+2] = 0; m[3*4+3] = 1

#define matrixfx_copy(mto, mfrom) \
matrix_copy(mto, mfrom)
//memcpy(mto, mfrom, sizeof(GLfixed) * 16)
#define matrixfx_clear(m) \
matrix_clear(m)
//memset(m, 0, sizeof(GLfixed) * 16)
#define matrixfx_identity(m) \
m[0*4+0] = gl_fpfromi(1); m[0*4+1] = gl_fpfromi(0); m[0*4+2] = gl_fpfromi(0); m[0*4+3] = gl_fpfromi(0); \
m[1*4+0] = gl_fpfromi(0); m[1*4+1] = gl_fpfromi(1); m[1*4+2] = gl_fpfromi(0); m[1*4+3] = gl_fpfromi(0); \
m[2*4+0] = gl_fpfromi(0); m[2*4+1] = gl_fpfromi(0); m[2*4+2] = gl_fpfromi(1); m[2*4+3] = gl_fpfromi(0); \
m[3*4+0] = gl_fpfromi(0); m[3*4+1] = gl_fpfromi(0); m[3*4+2] = gl_fpfromi(0); m[3*4+3] = gl_fpfromi(1)


template <class T> bool  TMatrix4x4<T>::bInitialized_(false);
template <class T> T     TMatrix4x4<T>::sinTable_[360];
template <class T> T     TMatrix4x4<T>::cosTable_[360];


//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4(const TMatrix4x4 & m)
{
  *this = m;
}

//---------------------------------------------------------------------------
template <class T>
inline
TMatrix4x4<T>::TMatrix4x4(const T * m)
{
  *this = m;
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
  m[0*4+3] = x;
  m[1*4+3] = y;
  m[2*4+3] = z;
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
  m[0*4+0] = x;
  m[1*4+1] = y;
  m[2*4+2] = z;
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
TMatrix4x4<T>::rotate(T x, T y, T z)
{
  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i(0); i < 360; i++)
    {
      sinTable_[i] = sin((float)i * M_PI / 180.0f);
      cosTable_[i] = cos((float)i * M_PI / 180.0f);
    }
  }

  if(x != 0)
    rotatex(x);
  if(y != 0)
    rotatey(y);
  if(z != 0)
    rotatez(z);
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatex(T angle)
{
  // Normalize the angle
  angle -= ((int)angle / 360) * 360;
  if(angle < 0)
    angle += 360;
  // Get sin and cos from lookup table
  T iSin = sinTable_[(int)angle];
  T iCos = cosTable_[(int)angle];

  T m[16];
  matrix_identity(m);
  m[1*4+1] = iCos;
  m[1*4+2] = 0 - iSin;
  m[2*4+1] = iSin;
  m[2*4+2] = iCos;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatey(T angle)
{
  // Normalize the angle
  angle -= ((int)angle / 360) * 360;
  if(angle < 0)
    angle += 360;
  // Get sin and cos from lookup table
  T iSin = sinTable_[(int)angle];
  T iCos = cosTable_[(int)angle];

  T m[16];
  matrix_identity(m);
  m[0*4+0] = iCos;
  m[0*4+2] = iSin;
  m[2*4+0] = 0 - iSin;
  m[2*4+2] = iCos;
  *this *= m;
}

//---------------------------------------------------------------------------
template <class T>
inline void
TMatrix4x4<T>::rotatez(T angle)
{
  // Normalize the angle
  angle -= ((int)angle / 360) * 360;
  if(angle < 0)
    angle += 360;
  // Get sin and cos from lookup table
  T iSin = sinTable_[(int)angle];
  T iCos = cosTable_[(int)angle];

  T m[16];
  matrix_identity(m);
  m[0*4+0] = iCos;
  m[0*4+1] = iSin;
  m[1*4+0] = 0 - iSin;
  m[1*4+1] = iCos;
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
