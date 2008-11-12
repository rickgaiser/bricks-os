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


#ifndef GL_MATRIX_H
#define GL_MATRIX_H


#include "context.h"
#include "vhl/fixedPoint.h"
#include "vhl/matrix.h"
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

  virtual void glPopMatrix(void);
  virtual void glPushMatrix(void);

  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);

protected:
  GLenum                matrixMode_;

  // ModelView
  TMatrix4x4<GLfloat>   stackModelView[16];  // Stack
  int                   iCurrentModelView_;  // Current index
  TMatrix4x4<GLfloat>   matrixModelView;

  // Projection
  TMatrix4x4<GLfloat>   stackProjection[2];  // Stack
  int                   iCurrentProjection_; // Current index
  TMatrix4x4<GLfloat>   matrixProjection;

  // Texture
  TMatrix4x4<GLfloat>   stackTexture[2];     // Stack
  int                   iCurrentTexture_;    // Current index
  TMatrix4x4<GLfloat>   matrixTexture;

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

  virtual void glPopMatrix(void);
  virtual void glPushMatrix(void);

  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);

protected:
  GLenum               matrixMode_;

  // ModelView
  TMatrix4x4<CFixed>   stackModelView[16];  // Stack
  int                  iCurrentModelView_;  // Current index
  TMatrix4x4<CFixed>   matrixModelView;

  // Projection
  TMatrix4x4<CFixed>   stackProjection[2];  // Stack
  int                  iCurrentProjection_; // Current index
  TMatrix4x4<CFixed>   matrixProjection;

  // Texture
  TMatrix4x4<CFixed>   stackTexture[2];     // Stack
  int                  iCurrentTexture_;    // Current index
  TMatrix4x4<CFixed>   matrixTexture;

  TMatrix4x4<CFixed> * pCurrentMatrix_;
};
#endif // CONFIG_FPU


#endif
