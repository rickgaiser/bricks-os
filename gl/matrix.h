#ifndef CMATRIX_H
#define CMATRIX_H


#include "context.h"
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

//-----------------------------------------------------------------------------
class CAGLESMatrixF
 : public virtual IGLESContext
{
public:
  CAGLESMatrixF();
  virtual ~CAGLESMatrixF();

  virtual void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  virtual void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z);
  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z);

  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);

protected:
  GLenum      matrixMode_;
  CMatrixF    matrixModelView;
  CMatrixF    matrixProjection;
  CMatrixF    matrixPerspective;
  CMatrixF    matrixRotation;
  CMatrixF  * pCurrentMatrix_;

  GLfloat     zNear_;
  GLfloat     zFar_;
  GLfloat     zA_;
  GLfloat     zB_;
};

//-----------------------------------------------------------------------------
class CAGLESMatrixFx
 : public virtual IGLESContext
{
public:
  CAGLESMatrixFx();
  virtual ~CAGLESMatrixFx();

  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);

  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);

protected:
  GLenum      matrixMode_;
  CMatrixFx   matrixModelView;
  CMatrixFx   matrixProjection;
  CMatrixFx   matrixPerspective;
  CMatrixFx   matrixRotation;
  CMatrixFx * pCurrentMatrix_;

  GLfixed     zNear_;
  GLfixed     zFar_;
  GLfixed     zA_;
  GLfixed     zB_;
};


#endif
