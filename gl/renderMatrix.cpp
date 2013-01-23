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


#include "renderMatrix.h"
#include "CFuncDebug.h"
#include "string.h"
#include "math.h"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLMatrixFloat::CAGLMatrixFloat()
 : matrixMode_(GL_MODELVIEW)
 , iModelViewIndex_(0)
 , iProjectionIndex_(0)
 , iTextureIndex_(0)
{
  FUNCTION_DEBUG();

  TMatrix4x4<GLfloat>::init();

  // All stacks contain 1 identity matrix
  stackModelView[0].loadIdentity();
  stackProjection[0].loadIdentity();
  stackTexture[0].loadIdentity();

  // Setup current pointers
  pCurrentModelView_  = &stackModelView[0];
  pCurrentProjection_ = &stackProjection[0];
  pCurrentTexture_    = &stackTexture[0];
  pCurrentMatrix_     = pCurrentModelView_;
}

//---------------------------------------------------------------------------
CAGLMatrixFloat::~CAGLMatrixFloat()
{
  FUNCTION_DEBUG();
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  FUNCTION_DEBUG();

  GLfloat m[16] __attribute__ ((aligned (16)));

  GLfloat idw = 1.0f / (right - left);
  GLfloat idh = 1.0f / (top - bottom);
  GLfloat idz = 1.0f / (zFar - zNear);

  m[ 0] = (2.0f * zNear) * idw;
  m[ 1] = 0.0f;
  m[ 2] = (right + left) * idw;
  m[ 3] = 0.0f;

  m[ 4] = 0.0f;
  m[ 5] = (2.0f * zNear) * idh;
  m[ 6] = (top + bottom) * idh;
  m[ 7] = 0.0f;

  m[ 8] = 0.0f;
  m[ 9] = 0.0f;
  m[10] = -((zFar + zNear) * idz);
  m[11] = -((2.0f * zFar * zNear) * idz);

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = -1.0f;
  m[15] = 0.0f;

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glLoadMatrixf(const GLfloat *m)
{
  FUNCTION_DEBUG();

  *pCurrentMatrix_ = m;
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glMultMatrixf(const GLfloat *m)
{
  FUNCTION_DEBUG();

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  FUNCTION_DEBUG();

  GLfloat m[16] __attribute__ ((aligned (16)));

  GLfloat idw = 1.0f / (right - left);
  GLfloat idh = 1.0f / (top - bottom);
  GLfloat idz = 1.0f / (zFar - zNear);

  m[ 0] = 2.0f * idw;
  m[ 1] = 0.0f;
  m[ 2] = 0.0f;
  m[ 3] = -((right + left) * idw);

  m[ 4] = 0.0f;
  m[ 5] = 2.0f * idh;
  m[ 6] = 0.0f;
  m[ 7] = -((top + bottom) * idh);

  m[ 8] = 0.0f;
  m[ 9] = 0.0f;
  m[10] = -2.0f * idz;
  m[11] = -((zFar + zNear) * idz);

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  FUNCTION_DEBUG();

  pCurrentMatrix_->rotate(angle, x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  FUNCTION_DEBUG();

  pCurrentMatrix_->scale(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  FUNCTION_DEBUG();

  pCurrentMatrix_->translate(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glPopMatrix(void)
{
  FUNCTION_DEBUG();

  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      if(iModelViewIndex_ > 0)
      {
        pCurrentModelView_ = &stackModelView[iModelViewIndex_ - 1];
        iModelViewIndex_--;
        pCurrentMatrix_ = pCurrentModelView_;
      }
      break;
    case GL_PROJECTION:
      if(iProjectionIndex_ > 0)
      {
        pCurrentProjection_ = &stackProjection[iProjectionIndex_ - 1];
        iProjectionIndex_--;
        pCurrentMatrix_ = pCurrentProjection_;
      }
      break;
    case GL_TEXTURE:
      if(iTextureIndex_ > 0)
      {
        pCurrentTexture_ = &stackTexture[iTextureIndex_ - 1];
        iTextureIndex_--;
        pCurrentMatrix_ = pCurrentTexture_;
      }
      break;
  }
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glPushMatrix(void)
{
  FUNCTION_DEBUG();

  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      if((iModelViewIndex_ + 1) < GL_MATRIX_MODELVIEW_STACK_SIZE)
      {
        stackModelView[iModelViewIndex_ + 1] = *pCurrentModelView_;
        iModelViewIndex_++;
        pCurrentModelView_ = &stackModelView[iModelViewIndex_];
        pCurrentMatrix_ = pCurrentModelView_;
      }
      break;
    case GL_PROJECTION:
      if((iProjectionIndex_ + 1) < GL_MATRIX_PROJECTION_STACK_SIZE)
      {
        stackProjection[iProjectionIndex_ + 1] = *pCurrentProjection_;
        iProjectionIndex_++;
        pCurrentProjection_ = &stackProjection[iProjectionIndex_];
        pCurrentMatrix_ = pCurrentProjection_;
      }
      break;
    case GL_TEXTURE:
      if((iTextureIndex_ + 1) < GL_MATRIX_TEXTURE_STACK_SIZE)
      {
        stackTexture[iTextureIndex_ + 1] = *pCurrentTexture_;
        iTextureIndex_++;
        pCurrentTexture_ = &stackTexture[iTextureIndex_];
        pCurrentMatrix_ = pCurrentTexture_;
      }
      break;
  }
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glLoadIdentity(void)
{
  FUNCTION_DEBUG();

  pCurrentMatrix_->loadIdentity();
}

//---------------------------------------------------------------------------
void
CAGLMatrixFloat::glMatrixMode(GLenum mode)
{
  FUNCTION_DEBUG();

  matrixMode_ = mode;

  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      pCurrentModelView_ = &stackModelView[iModelViewIndex_];
      pCurrentMatrix_ = pCurrentModelView_;
      break;
    case GL_PROJECTION:
      pCurrentProjection_ = &stackProjection[iProjectionIndex_];
      pCurrentMatrix_ = pCurrentProjection_;
      break;
    case GL_TEXTURE:
      pCurrentTexture_ = &stackTexture[iTextureIndex_];
      pCurrentMatrix_ = pCurrentTexture_;
      break;
  }
}
