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


#ifndef CMATRIX_H
#define CMATRIX_H


#include "inttypes.h"
#include "vector.h"


#define MATRIX_MATH_TABLE_BITS (10) // 10bits == 1024 values
#define MATRIX_MATH_TABLE_SIZE 1024 // (1<<MATRIX_MATH_TABLE_BITS) <-- Does not work with GCC 3.2.2

#define ROW_MAJOR
//#define COLUMN_MAJOR // OpenGL


//---------------------------------------------------------------------------
template <class T>
class TMatrix4x4
{
public:
#ifdef ROW_MAJOR
  static const int ROW0 = 0*4;
  static const int ROW1 = 1*4;
  static const int ROW2 = 2*4;
  static const int ROW3 = 3*4;
  static const int COL0 = 0;
  static const int COL1 = 1;
  static const int COL2 = 2;
  static const int COL3 = 3;
#else
  static const int ROW0 = 0;
  static const int ROW1 = 1;
  static const int ROW2 = 2;
  static const int ROW3 = 3;
  static const int COL0 = 0*4;
  static const int COL1 = 1*4;
  static const int COL2 = 2*4;
  static const int COL3 = 3*4;
#endif
  static const int RC00 = ROW0 + COL0;
  static const int RC01 = ROW0 + COL1;
  static const int RC02 = ROW0 + COL2;
  static const int RC03 = ROW0 + COL3;
  static const int RC10 = ROW1 + COL0;
  static const int RC11 = ROW1 + COL1;
  static const int RC12 = ROW1 + COL2;
  static const int RC13 = ROW1 + COL3;
  static const int RC20 = ROW2 + COL0;
  static const int RC21 = ROW2 + COL1;
  static const int RC22 = ROW2 + COL2;
  static const int RC23 = ROW2 + COL3;
  static const int RC30 = ROW3 + COL0;
  static const int RC31 = ROW3 + COL1;
  static const int RC32 = ROW3 + COL2;
  static const int RC33 = ROW3 + COL3;

public:
  // Constructors
  TMatrix4x4();
  TMatrix4x4(const TMatrix4x4 & m);
  TMatrix4x4(const T * m);
  TMatrix4x4(T _m00, T _m01, T _m02, T _m03,
             T _m10, T _m11, T _m12, T _m13,
             T _m20, T _m21, T _m22, T _m23,
             T _m30, T _m31, T _m32, T _m33);

  // Initialize the sin and cos tables
  static void init();

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

  void rotate    (T angle, T x, T y, T z);
  void rotatex   (T angle);
  void rotatey   (T angle);
  void rotatez   (T angle);

  void transform3(const T * from, T * to);
  void transform3(const TVector3<T> & from, TVector3<T> & to);
  void transform3(const TVector4<T> & from, TVector4<T> & to);
  void transform4(const T * from, T * to);
  void transform4(const TVector4<T> & from, TVector4<T> & to);

public:
  T matrix[16];

private:
  static bool bInitialized_;
  static T sinTable_[MATRIX_MATH_TABLE_SIZE];
  static T cosTable_[MATRIX_MATH_TABLE_SIZE];
};


#include "matrix.inl"


#endif
