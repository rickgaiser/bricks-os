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


#include "glMatrix.h"
#include "string.h"
#include "math.h"


#define POP_MATRIX()                                                 \
switch(matrixMode_)                                                  \
{                                                                    \
  case GL_MODELVIEW:                                                 \
    if(iModelViewIndex_ > 0)                                         \
    {                                                                \
      pCurrentModelView_ = &stackModelView[iModelViewIndex_ - 1];    \
      iModelViewIndex_--;                                            \
      pCurrentMatrix_ = pCurrentModelView_;                          \
    }                                                                \
    break;                                                           \
  case GL_PROJECTION:                                                \
    if(iProjectionIndex_ > 0)                                        \
    {                                                                \
      pCurrentProjection_ = &stackProjection[iProjectionIndex_ - 1]; \
      iProjectionIndex_--;                                           \
      pCurrentMatrix_ = pCurrentProjection_;                         \
    }                                                                \
    break;                                                           \
  case GL_TEXTURE:                                                   \
    if(iTextureIndex_ > 0)                                           \
    {                                                                \
      pCurrentTexture_ = &stackTexture[iTextureIndex_ - 1];          \
      iTextureIndex_--;                                              \
      pCurrentMatrix_ = pCurrentTexture_;                            \
    }                                                                \
    break;                                                           \
}

#define PUSH_MATRIX()                                                \
switch(matrixMode_)                                                  \
{                                                                    \
  case GL_MODELVIEW:                                                 \
    if((iModelViewIndex_ + 1) < GL_MATRIX_MODELVIEW_STACK_SIZE)      \
    {                                                                \
      stackModelView[iModelViewIndex_ + 1] = *pCurrentModelView_;    \
      iModelViewIndex_++;                                            \
      pCurrentModelView_ = &stackModelView[iModelViewIndex_];        \
      pCurrentMatrix_ = pCurrentModelView_;                          \
    }                                                                \
    break;                                                           \
  case GL_PROJECTION:                                                \
    if((iProjectionIndex_ + 1) < GL_MATRIX_PROJECTION_STACK_SIZE)    \
    {                                                                \
      stackProjection[iProjectionIndex_ + 1] = *pCurrentProjection_; \
      iProjectionIndex_++;                                           \
      pCurrentProjection_ = &stackProjection[iProjectionIndex_];     \
      pCurrentMatrix_ = pCurrentProjection_;                         \
    }                                                                \
    break;                                                           \
  case GL_TEXTURE:                                                   \
    if((iTextureIndex_ + 1) < GL_MATRIX_TEXTURE_STACK_SIZE)          \
    {                                                                \
      stackTexture[iTextureIndex_ + 1] = *pCurrentTexture_;          \
      iTextureIndex_++;                                              \
      pCurrentTexture_ = &stackTexture[iTextureIndex_];              \
      pCurrentMatrix_ = pCurrentTexture_;                            \
    }                                                                \
    break;                                                           \
}

#define MATRIX_MODE()                                          \
matrixMode_ = mode;                                            \
switch(matrixMode_)                                                   \
{                                                              \
  case GL_MODELVIEW:                                           \
    pCurrentModelView_ = &stackModelView[iModelViewIndex_];    \
    pCurrentMatrix_ = pCurrentModelView_;                      \
    break;                                                     \
  case GL_PROJECTION:                                          \
    pCurrentProjection_ = &stackProjection[iProjectionIndex_]; \
    pCurrentMatrix_ = pCurrentProjection_;                     \
    break;                                                     \
  case GL_TEXTURE:                                             \
    pCurrentTexture_ = &stackTexture[iTextureIndex_];          \
    pCurrentMatrix_ = pCurrentTexture_;                        \
    break;                                                     \
}

#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixF::CAGLESMatrixF()
 : matrixMode_(GL_MODELVIEW)
 , iModelViewIndex_(0)
 , iProjectionIndex_(0)
 , iTextureIndex_(0)
{
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
CAGLESMatrixF::~CAGLESMatrixF()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  GLfloat m[16];

  GLfloat idw = 1.0f / (right - left);
  GLfloat idh = 1.0f / (top - bottom);
  GLfloat idz = 1.0f / (zFar - zNear);

  m[TMatrix4x4<GLfloat>::RC00] = (2.0f * zNear) * idw;
  m[TMatrix4x4<GLfloat>::RC01] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC02] = (right + left) * idw;
  m[TMatrix4x4<GLfloat>::RC03] = 0.0f;

  m[TMatrix4x4<GLfloat>::RC10] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC11] = (2.0f * zNear) * idh;
  m[TMatrix4x4<GLfloat>::RC12] = (top + bottom) * idh;
  m[TMatrix4x4<GLfloat>::RC13] = 0.0f;

  m[TMatrix4x4<GLfloat>::RC20] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC21] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC22] = -((zFar + zNear) * idz);
  m[TMatrix4x4<GLfloat>::RC23] = -((2.0f * zFar * zNear) * idz);

  m[TMatrix4x4<GLfloat>::RC30] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC31] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC32] = -1.0f;
  m[TMatrix4x4<GLfloat>::RC33] = 0.0f;

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glLoadMatrixf(const GLfloat *m)
{
  *pCurrentMatrix_ = m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glMultMatrixf(const GLfloat *m)
{
  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  GLfloat m[16];

  GLfloat idw = 1.0f / (right - left);
  GLfloat idh = 1.0f / (top - bottom);
  GLfloat idz = 1.0f / (zFar - zNear);

  m[TMatrix4x4<GLfloat>::RC00] = 2.0f * idw;
  m[TMatrix4x4<GLfloat>::RC01] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC02] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC03] = -((right + left) * idw);

  m[TMatrix4x4<GLfloat>::RC10] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC11] = 2.0f * idh;
  m[TMatrix4x4<GLfloat>::RC12] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC13] = -((top + bottom) * idh);

  m[TMatrix4x4<GLfloat>::RC20] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC21] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC22] = -2.0f * idz;
  m[TMatrix4x4<GLfloat>::RC23] = -((zFar + zNear) * idz);

  m[TMatrix4x4<GLfloat>::RC30] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC31] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC32] = 0.0f;
  m[TMatrix4x4<GLfloat>::RC33] = 1.0f;

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->scale(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->translate(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glPopMatrix(void)
{
  POP_MATRIX();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glPushMatrix(void)
{
  PUSH_MATRIX();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glLoadIdentity(void)
{
  pCurrentMatrix_->loadIdentity();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glMatrixMode(GLenum mode)
{
  MATRIX_MODE();
}
#else // CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixFx::CAGLESMatrixFx()
 : matrixMode_(GL_MODELVIEW)
 , iModelViewIndex_(0)
 , iProjectionIndex_(0)
 , iTextureIndex_(0)
{
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
CAGLESMatrixFx::~CAGLESMatrixFx()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  CFixed m[16];

  GLfixed idw = gl_fpinverse(right - left);
  GLfixed idh = gl_fpinverse(top - bottom);
  GLfixed idz = gl_fpinverse(zFar - zNear);

  m[TMatrix4x4<CFixed>::RC00].value = gl_fpmul((zNear << 1), idw);
  m[TMatrix4x4<CFixed>::RC01].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC02].value = gl_fpmul((right + left), idw);
  m[TMatrix4x4<CFixed>::RC03].value = gl_fpfromi(0);

  m[TMatrix4x4<CFixed>::RC10].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC11].value = gl_fpmul((zNear << 1), idh);
  m[TMatrix4x4<CFixed>::RC12].value = gl_fpmul((top + bottom), idh);
  m[TMatrix4x4<CFixed>::RC13].value = gl_fpfromi(0);

  m[TMatrix4x4<CFixed>::RC20].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC21].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC22].value = -gl_fpmul((zFar + zNear), idz);
  m[TMatrix4x4<CFixed>::RC23].value = -gl_fpmul((gl_fpmul(zFar, zNear) << 1), idz);

  m[TMatrix4x4<CFixed>::RC30].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC31].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC32].value = gl_fpfromi(-1);
  m[TMatrix4x4<CFixed>::RC33].value = gl_fpfromi(0);

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glLoadMatrixx(const GLfixed * m)
{
  *pCurrentMatrix_ = (CFixed *)m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glMultMatrixx(const GLfixed * m)
{
  *pCurrentMatrix_ *= (CFixed *)m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  CFixed m[16];

  GLfixed idw = gl_fpinverse(right - left);
  GLfixed idh = gl_fpinverse(top - bottom);
  GLfixed idz = gl_fpinverse(zFar - zNear);

  m[TMatrix4x4<CFixed>::RC00].value = gl_fpmul(gl_fpfromi(2), idw);
  m[TMatrix4x4<CFixed>::RC01].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC02].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC03].value = -gl_fpmul((right + left), idw);

  m[TMatrix4x4<CFixed>::RC10].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC11].value = gl_fpmul(gl_fpfromi(2), idh);
  m[TMatrix4x4<CFixed>::RC12].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC13].value = -gl_fpmul((top + bottom), idh);

  m[TMatrix4x4<CFixed>::RC20].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC21].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC22].value = gl_fpmul(gl_fpfromi(-2), idz);
  m[TMatrix4x4<CFixed>::RC23].value = -gl_fpmul((zFar + zNear), idz);

  m[TMatrix4x4<CFixed>::RC30].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC31].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC32].value = gl_fpfromi(0);
  m[TMatrix4x4<CFixed>::RC33].value = gl_fpfromi(1);

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  CFixed fa, fx, fy, fz;

  fa.value = angle;
  fx.value = x;
  fy.value = y;
  fz.value = z;

  pCurrentMatrix_->rotate(fa, fx, fy, fz);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  CFixed fx, fy, fz;

  fx.value = x;
  fy.value = y;
  fz.value = z;

  pCurrentMatrix_->scale(fx, fy, fz);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  CFixed fx, fy, fz;

  fx.value = x;
  fy.value = y;
  fz.value = z;

  pCurrentMatrix_->translate(fx, fy, fz);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glPopMatrix(void)
{
  POP_MATRIX();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glPushMatrix(void)
{
  PUSH_MATRIX();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glLoadIdentity(void)
{
  pCurrentMatrix_->loadIdentity();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glMatrixMode(GLenum mode)
{
  MATRIX_MODE();
}
#endif // CONFIG_FPU
