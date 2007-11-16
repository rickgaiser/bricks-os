#ifndef CMATRIX_H
#define CMATRIX_H


#include "context.h"
#include "fixedPoint.h"
#include "asm/arch/config.h"


#define MATRIX_PRECISION   16
#define m_fpfromi(i)   fpfromi(MATRIX_PRECISION,i)
#define m_fptoi(i)     fptoi(MATRIX_PRECISION,i)
#define m_fpfromf(i)   fpfromf(MATRIX_PRECISION,i)
#define m_fptof(i)     fptof(MATRIX_PRECISION,i)
#define m_fpmul(i1,i2) fpmul32(MATRIX_PRECISION,i1,i2)
#define m_fpdiv(i1,i2) fpdiv32(MATRIX_PRECISION,i1,i2)

#define m_to_gl(i)     (i)
#define gl_to_m(i)     (i)


typedef GLfixed Mfixed;


#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
class CMatrixF
{
public:
  CMatrixF();
  virtual ~CMatrixF();

  void clear();
  void loadIdentity();

  void frustum  (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  void ortho    (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);

  void translate(GLfloat x, GLfloat y, GLfloat z);
  void translate(GLfloat * vec);

  void scale    (GLfloat x, GLfloat y, GLfloat z);
  void scale    (GLfloat * vec);

  void rotate   (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  void rotate   (GLfloat * angles);

  void transform(const GLfloat * from, GLfloat * to);

  CMatrixF   operator* (const CMatrixF & m);
  CMatrixF   operator* (const GLfloat * m);

  CMatrixF & operator*=(const CMatrixF & m);
  CMatrixF & operator*=(const GLfloat * m);

  CMatrixF & operator= (const CMatrixF & m);
  CMatrixF & operator= (const GLfloat * m);

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
#else
//---------------------------------------------------------------------------
class CMatrixFx
{
public:
  CMatrixFx();
  virtual ~CMatrixFx();

  void clear();
  void loadIdentity();

  void frustum  (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  void ortho    (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);

  void translate(GLfixed x, GLfixed y, GLfixed z);
  void translate(GLfixed * vec);

  void scale    (GLfixed x, GLfixed y, GLfixed z);
  void scale    (GLfixed * vec);

  void rotate   (GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  void rotate   (GLfixed * angles);

  void transform(const GLfixed * from, GLfixed * to);

  CMatrixFx   operator* (const CMatrixFx & m);
  CMatrixFx   operator* (const GLfixed * m);

  CMatrixFx & operator*=(const CMatrixFx & m);
  CMatrixFx & operator*=(const GLfixed * m);

  CMatrixFx & operator= (const CMatrixFx & m);
  CMatrixFx & operator= (const GLfixed * m);

public:
  Mfixed matrix[4][4];
  // [ROW][COLUMN]
  // 00, 01, 02, 03
  // 10, 11, 12, 13
  // 20, 21, 22, 23
  // 30, 31, 32, 33

private:
  static bool bInitialized_;
  static Mfixed fpSin_[360];
  static Mfixed fpCos_[360];
};
#endif // CONFIG_FPU

#ifdef CONFIG_FPU
//-----------------------------------------------------------------------------
class CAGLESMatrixF
 : public virtual IGLESContext
{
public:
  CAGLESMatrixF();
  virtual ~CAGLESMatrixF();

  virtual void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  virtual void glLoadMatrixf(const GLfloat *m);
  virtual void glMultMatrixf(const GLfloat *m);
  virtual void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  virtual void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z);
  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z);

  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);

protected:
  GLenum      matrixMode_;
  CMatrixF    matrixModelView;
  CMatrixF    matrixProjection;
  CMatrixF    matrixTexture;
  CMatrixF  * pCurrentMatrix_;
};
#else
//-----------------------------------------------------------------------------
class CAGLESMatrixFx
 : public virtual IGLESContext
{
public:
  CAGLESMatrixFx();
  virtual ~CAGLESMatrixFx();

  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glLoadMatrixx(const GLfixed *m);
  virtual void glMultMatrixx(const GLfixed *m);
  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);

  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);

protected:
  GLenum      matrixMode_;
  CMatrixFx   matrixModelView;
  CMatrixFx   matrixProjection;
  CMatrixFx   matrixTexture;
  CMatrixFx * pCurrentMatrix_;
};
#endif // CONFIG_FPU


#endif
