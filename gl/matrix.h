#ifndef CMATRIX_H
#define CMATRIX_H


#include "context.h"
#include "fixedPoint.h"
#include "vector.h"
#include "asm/arch/config.h"


#define m00 matrix[0*4+0]
#define m01 matrix[0*4+1]
#define m02 matrix[0*4+2]
#define m03 matrix[0*4+3]
#define m10 matrix[1*4+0]
#define m11 matrix[1*4+1]
#define m12 matrix[1*4+2]
#define m13 matrix[1*4+3]
#define m20 matrix[2*4+0]
#define m21 matrix[2*4+1]
#define m22 matrix[2*4+2]
#define m23 matrix[2*4+3]
#define m30 matrix[3*4+0]
#define m31 matrix[3*4+1]
#define m32 matrix[3*4+2]
#define m33 matrix[3*4+3]


//---------------------------------------------------------------------------
template <class T>
class TMatrix4x4
{
public:
  // Constructors
  TMatrix4x4(){}
  TMatrix4x4(const TMatrix4x4 & m);
  TMatrix4x4(const T * m);
  TMatrix4x4(T _m00, T _m01, T _m02, T _m03,
          T _m10, T _m11, T _m12, T _m13,
          T _m20, T _m21, T _m22, T _m23,
          T _m30, T _m31, T _m32, T _m33);

  // Assignment Operators
  TMatrix4x4 & operator= (const TMatrix4x4 & m);
  TMatrix4x4 & operator= (const T * m);

  // Initializers
  void clear();
  void loadIdentity();

  // Compound Assignment Operators
  TMatrix4x4 & operator*=(const TMatrix4x4 & m);
  TMatrix4x4 & operator*=(const T * m);

  // Binary Arithmetic Operators
  TMatrix4x4   operator* (const TMatrix4x4 & m);
  TMatrix4x4   operator* (const T * m);

  void translate (T x, T y, T z);
  void translate (const T * vec);

  void scale     (T x, T y, T z);
  void scale     (const T * vec);

  void rotate    (T x, T y, T z);
  void rotatex   (T angle);
  void rotatey   (T angle);
  void rotatez   (T angle);

  void transform3(const T * from, T * to);
  void transform3(const TVector<T> & from, TVector<T> & to);
  void transform4(const T * from, T * to);
  void transform4(const TVector<T> & from, TVector<T> & to);

public:
  T matrix[16];

private:
  static bool bInitialized_;
  static T sinTable_[360];
  static T cosTable_[360];
};


#include "matrix.inl"


#endif
