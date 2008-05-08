#ifndef GL_MATRIX_H
#define GL_MATRIX_H


#include "context.h"
#include "fixedPoint.h"
#include "matrix.h"
#include "asm/arch/config.h"


#ifdef CONFIG_FPU
//-----------------------------------------------------------------------------
class CAGLESMatrixF
 : public virtual I3DRenderer
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
  GLenum                matrixMode_;
  TMatrix4x4<GLfloat>   matrixModelView;
  TMatrix4x4<GLfloat>   matrixProjection;
  TMatrix4x4<GLfloat>   matrixTexture;
  TMatrix4x4<GLfloat>   matrixNormal;
  TMatrix4x4<GLfloat> * pCurrentMatrix_;
};
#else // CONFIG_FPU
//-----------------------------------------------------------------------------
class CAGLESMatrixFx
 : public virtual I3DRenderer
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
  GLenum               matrixMode_;
  TMatrix4x4<CFixed>   matrixModelView;
  TMatrix4x4<CFixed>   matrixProjection;
  TMatrix4x4<CFixed>   matrixTexture;
  TMatrix4x4<CFixed>   matrixNormal;
  TMatrix4x4<CFixed> * pCurrentMatrix_;
};
#endif // CONFIG_FPU


#endif
