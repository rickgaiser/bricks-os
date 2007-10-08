#ifndef CMATRIX_H
#define CMATRIX_H


#include "fixedPoint.h"


#define MATRIX_PRECISION   16
#define m_fpfromi(i)   fpfromi(MATRIX_PRECISION,i)
#define m_fptoi(i)     fptoi(MATRIX_PRECISION,i)
#define m_fpfromf(i)   fpfromf(MATRIX_PRECISION,i)
#define m_fptof(i)     fptof(MATRIX_PRECISION,i)
#define m_fpmul(i1,i2) fpmul32(MATRIX_PRECISION,i1,i2)
#define m_fpdiv(i1,i2) fpdiv32(MATRIX_PRECISION,i1,i2)


//---------------------------------------------------------------------------
class CMatrixF
{
public:
  CMatrixF();
  virtual ~CMatrixF();

  void clear();
  void loadIdentity();

  void translate(GLfloat x, GLfloat y, GLfloat z);
  void translate(GLfloat * vec);

  void scale    (GLfloat x, GLfloat y, GLfloat z);
  void scale    (GLfloat * vec);

  void rotate   (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  void rotate   (GLfloat * angles);

  void transform(const GLfloat * from, GLfloat * to);

  CMatrixF   operator* (const CMatrixF & m);
  CMatrixF & operator*=(const CMatrixF & m);

public:
  GLfloat matrix[4][4];
  // [ROW][COLUMN]
  // 00, 01, 02, 03
  // 10, 11, 12, 13
  // 20, 21, 22, 23
  // 30, 31, 32, 33

private:
  static bool bInitialized_;
  static GLfloat fpSin_[360];
  static GLfloat fpCos_[360];
};

//---------------------------------------------------------------------------
class CMatrixFx
{
public:
  CMatrixFx();
  virtual ~CMatrixFx();

  void clear();
  void loadIdentity();

  void translate(GLfixed x, GLfixed y, GLfixed z);
  void translate(GLfixed * vec);

  void scale    (GLfixed x, GLfixed y, GLfixed z);
  void scale    (GLfixed * vec);

  void rotate   (GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  void rotate   (GLfixed * angles);

  void transform(const GLfixed * from, GLfixed * to);

  CMatrixFx   operator* (const CMatrixFx & m);
  CMatrixFx & operator*=(const CMatrixFx & m);

public:
  GLfixed matrix[4][4];
  // [ROW][COLUMN]
  // 00, 01, 02, 03
  // 10, 11, 12, 13
  // 20, 21, 22, 23
  // 30, 31, 32, 33

private:
  static bool bInitialized_;
  static GLfixed fpSin_[360];
  static GLfixed fpCos_[360];
};


#endif
