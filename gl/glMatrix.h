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
#include "vhl/matrix.h"


#define GL_MATRIX_MODELVIEW_STACK_SIZE  32
#define GL_MATRIX_PROJECTION_STACK_SIZE  2
#define GL_MATRIX_TEXTURE_STACK_SIZE     2


//-----------------------------------------------------------------------------
class CAGLMatrixFloat
 : public virtual I3DRenderer
{
public:
  CAGLMatrixFloat();
  virtual ~CAGLMatrixFloat();

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
  TMatrix4x4<GLfloat>   stackModelView[GL_MATRIX_MODELVIEW_STACK_SIZE];
  TMatrix4x4<GLfloat> * pCurrentModelView_;
  int                   iModelViewIndex_;

  // Projection
  TMatrix4x4<GLfloat>   stackProjection[GL_MATRIX_PROJECTION_STACK_SIZE];
  TMatrix4x4<GLfloat> * pCurrentProjection_;
  int                   iProjectionIndex_;

  // Texture
  TMatrix4x4<GLfloat>   stackTexture[GL_MATRIX_TEXTURE_STACK_SIZE];
  TMatrix4x4<GLfloat> * pCurrentTexture_;
  int                   iTextureIndex_;

  TMatrix4x4<GLfloat> * pCurrentMatrix_;
};


#endif
