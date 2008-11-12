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


#include "context.h"
#include "vhl/fixedPoint.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CAGLESBase::CAGLESBase()
 : bBufColorEnabled_(false)
 , bBufNormalEnabled_(false)
 , bBufTexCoordEnabled_(false)
 , bBufVertexEnabled_(false)
 , cullFaceEnabled_(false)
 , cullFaceMode_(GL_BACK)
 , frontFace_(GL_CCW)
 , bCullCW_(true)
 , hintFog_(GL_DONT_CARE)
 , hintLineSmooth_(GL_DONT_CARE)
 , hintPerspectiveCorrection_(GL_DONT_CARE)
 , hintPointSmooth_(GL_DONT_CARE)
 , errorCode_(GL_NO_ERROR)
 , bError_(false)
{
}

//-----------------------------------------------------------------------------
CAGLESBase::~CAGLESBase()
{
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  if((size != 4) || (stride < 0))
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  switch(type)
  {
//    case GL_UNSIGNED_BYTE:
//      break;
    case GL_FIXED:
      break;
    case GL_FLOAT:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  bufColor_.size    = size;
  bufColor_.type    = type;
  bufColor_.stride  = stride;
  bufColor_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glDisableClientState(GLenum array)
{
  switch(array)
  {
    case GL_COLOR_ARRAY:         bBufColorEnabled_    = false; break;
    case GL_NORMAL_ARRAY:        bBufNormalEnabled_   = false; break;
    case GL_TEXTURE_COORD_ARRAY: bBufTexCoordEnabled_ = false; break;
    case GL_VERTEX_ARRAY:        bBufVertexEnabled_   = false; break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glEnableClientState(GLenum array)
{
  switch(array)
  {
    case GL_COLOR_ARRAY:         bBufColorEnabled_    = true; break;
    case GL_NORMAL_ARRAY:        bBufNormalEnabled_   = true; break;
    case GL_TEXTURE_COORD_ARRAY: bBufTexCoordEnabled_ = true; break;
    case GL_VERTEX_ARRAY:        bBufVertexEnabled_   = true; break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
  if(stride < 0)
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  switch(type)
  {
//    case GL_BYTE:
//      break;
//    case GL_SHORT:
//      break;
    case GL_FIXED:
      break;
    case GL_FLOAT:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  bufNormal_.size    = 3;  // x,y,z
  bufNormal_.type    = type;
  bufNormal_.stride  = stride;
  bufNormal_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  if((size < 2) || (size > 4) || (stride < 0))
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  switch(type)
  {
//    case GL_BYTE:
//      break;
//    case GL_SHORT:
//      break;
    case GL_FIXED:
      break;
    case GL_FLOAT:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  bufTexCoord_.size    = size;
  bufTexCoord_.type    = type;
  bufTexCoord_.stride  = stride;
  bufTexCoord_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  if((size < 2) || (size > 4) || (stride < 0))
  {
    setError(GL_INVALID_VALUE);
    return;
  }

  switch(type)
  {
//    case GL_BYTE:
//      break;
//    case GL_SHORT:
//      break;
    case GL_FIXED:
      break;
    case GL_FLOAT:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  bufVertex_.size    = size;
  bufVertex_.type    = type;
  bufVertex_.stride  = stride;
  bufVertex_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glCullFace(GLenum mode)
{
  cullFaceMode_ = mode;

  bCullCW_ = (frontFace_ == GL_CCW) == (cullFaceMode_ == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glFrontFace(GLenum mode)
{
  frontFace_ = mode;

  bCullCW_ = (frontFace_ == GL_CCW) == (cullFaceMode_ == GL_BACK);
}

//-----------------------------------------------------------------------------
void
CAGLESBase::glHint(GLenum target, GLenum mode)
{
  GLenum * pHint;

  switch(target)
  {
    case GL_FOG_HINT:
      pHint = &hintFog_;
      break;
    case GL_LINE_SMOOTH_HINT:
      pHint = &hintLineSmooth_;
      break;
    case GL_PERSPECTIVE_CORRECTION_HINT:
      pHint = &hintPerspectiveCorrection_;
      break;
    case GL_POINT_SMOOTH_HINT:
      pHint = &hintPointSmooth_;
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  switch(mode)
  {
    case GL_FASTEST:
      break;
    case GL_NICEST:
      break;
    case GL_DONT_CARE:
      break;
    default:
      setError(GL_INVALID_ENUM);
      return;
  };

  *pHint = mode;
}

//-----------------------------------------------------------------------------
GLenum
CAGLESBase::glGetError(void)
{
  GLenum err(errorCode_);

  bError_ = false;
  errorCode_ = GL_NO_ERROR;

  return err;
}

//-----------------------------------------------------------------------------
void
CAGLESBase::setError(GLenum error)
{
  if(bError_ == false)
  {
    bError_ = true;
    errorCode_ = error;
  }
}

#ifndef CONFIG_FPU
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  glClearColorx(gl_fpfromf(red), gl_fpfromf(green), gl_fpfromf(blue), gl_fpfromf(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glClearDepthf(GLclampf depth)
{
  glClearDepthx(gl_fpfromf(depth));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glDepthRangef(GLclampf zNear, GLclampf zFar)
{
  glDepthRangex(gl_fpfromf(zNear), gl_fpfromf(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glFogf(GLenum pname, GLfloat param)
{
  glFogx(pname, gl_fpfromf(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glFogfv(GLenum pname, const GLfloat * params)
{
  GLfixed xparams[] = {gl_fpfromf(params[0])
                     , gl_fpfromf(params[1])
                     , gl_fpfromf(params[2])
                     , gl_fpfromf(params[3])};

  glFogxv(pname, xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glLoadMatrixf(const GLfloat *m)
{
  GLfixed xparams[] = {gl_fpfromf(m[0])
                     , gl_fpfromf(m[1])
                     , gl_fpfromf(m[2])
                     , gl_fpfromf(m[3])
                     , gl_fpfromf(m[4])
                     , gl_fpfromf(m[5])
                     , gl_fpfromf(m[6])
                     , gl_fpfromf(m[7])
                     , gl_fpfromf(m[8])
                     , gl_fpfromf(m[9])
                     , gl_fpfromf(m[10])
                     , gl_fpfromf(m[11])
                     , gl_fpfromf(m[12])
                     , gl_fpfromf(m[13])
                     , gl_fpfromf(m[14])
                     , gl_fpfromf(m[15])};

  glLoadMatrixx(xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
  glMaterialx(face, pname, gl_fpfromf(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
  GLfixed xparams[] = {gl_fpfromf(params[0])
                     , gl_fpfromf(params[1])
                     , gl_fpfromf(params[2])
                     , gl_fpfromf(params[3])};

  glMaterialxv(face, pname, xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glMultMatrixf(const GLfloat *m)
{
  GLfixed xparams[] = {gl_fpfromf(m[0])
                     , gl_fpfromf(m[1])
                     , gl_fpfromf(m[2])
                     , gl_fpfromf(m[3])
                     , gl_fpfromf(m[4])
                     , gl_fpfromf(m[5])
                     , gl_fpfromf(m[6])
                     , gl_fpfromf(m[7])
                     , gl_fpfromf(m[8])
                     , gl_fpfromf(m[9])
                     , gl_fpfromf(m[10])
                     , gl_fpfromf(m[11])
                     , gl_fpfromf(m[12])
                     , gl_fpfromf(m[13])
                     , gl_fpfromf(m[14])
                     , gl_fpfromf(m[15])};

  glMultMatrixx(xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  glFrustumx(gl_fpfromf(left), gl_fpfromf(right), gl_fpfromf(bottom), gl_fpfromf(top), gl_fpfromf(zNear), gl_fpfromf(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glLightf(GLenum light, GLenum pname, GLfloat param)
{
  glLightx(light, pname, gl_fpfromf(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
  GLfixed xparams[] = {gl_fpfromf(params[0])
                     , gl_fpfromf(params[1])
                     , gl_fpfromf(params[2])
                     , gl_fpfromf(params[3])};

  glLightxv(light, pname, xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  glOrthox(gl_fpfromf(left), gl_fpfromf(right), gl_fpfromf(bottom), gl_fpfromf(top), gl_fpfromf(zNear), gl_fpfromf(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  glRotatex(gl_fpfromf(angle), gl_fpfromf(x), gl_fpfromf(y), gl_fpfromf(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  glScalex(gl_fpfromf(x), gl_fpfromf(y), gl_fpfromf(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  glTranslatex(gl_fpfromf(x), gl_fpfromf(y), gl_fpfromf(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  glVertex4x(gl_fpfromf(x), gl_fpfromf(y), gl_fpfromf(z), gl_fpfromf(w));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  glColor4x(gl_fpfromf(red), gl_fpfromf(green), gl_fpfromf(blue), gl_fpfromf(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
  glTexCoord4x(gl_fpfromf(s), gl_fpfromf(t), gl_fpfromf(r), gl_fpfromf(q));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  glNormal3x(gl_fpfromf(nx), gl_fpfromf(ny), gl_fpfromf(nz));
}
#else
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  glClearColor(gl_fptof(red), gl_fptof(green), gl_fptof(blue), gl_fptof(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glClearDepthx(GLclampx depth)
{
  glClearDepthf(gl_fptof(depth));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glDepthRangex(GLclampx zNear, GLclampx zFar)
{
  glDepthRangef(gl_fptof(zNear), gl_fptof(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glFogx(GLenum pname, GLfixed param)
{
  glFogf(pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glFogxv(GLenum pname, const GLfixed *params)
{
  GLfloat fparams[] = {gl_fptof(params[0])
                     , gl_fptof(params[1])
                     , gl_fptof(params[2])
                     , gl_fptof(params[3])};

  glFogfv(pname, fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  glFrustumf(gl_fptof(left), gl_fptof(right), gl_fptof(bottom), gl_fptof(top), gl_fptof(zNear), gl_fptof(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glLightx(GLenum light, GLenum pname, GLfixed param)
{
  glLightf(light, pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  GLfloat fparams[] = {gl_fptof(params[0])
                     , gl_fptof(params[1])
                     , gl_fptof(params[2])
                     , gl_fptof(params[3])};

  glLightfv(light, pname, fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glLoadMatrixx(const GLfixed *m)
{
  GLfloat fparams[] = {gl_fptof(m[0])
                     , gl_fptof(m[1])
                     , gl_fptof(m[2])
                     , gl_fptof(m[3])
                     , gl_fptof(m[4])
                     , gl_fptof(m[5])
                     , gl_fptof(m[6])
                     , gl_fptof(m[7])
                     , gl_fptof(m[8])
                     , gl_fptof(m[9])
                     , gl_fptof(m[10])
                     , gl_fptof(m[11])
                     , gl_fptof(m[12])
                     , gl_fptof(m[13])
                     , gl_fptof(m[14])
                     , gl_fptof(m[15])};

  glLoadMatrixf(fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
  glMaterialf(face, pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glMaterialxv(GLenum face, GLenum pname, const GLfixed * params)
{
  GLfloat fparams[] = {gl_fptof(params[0])
                     , gl_fptof(params[1])
                     , gl_fptof(params[2])
                     , gl_fptof(params[3])};

  glMaterialfv(face, pname, fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glMultMatrixx(const GLfixed *m)
{
  GLfloat fparams[] = {gl_fptof(m[0])
                     , gl_fptof(m[1])
                     , gl_fptof(m[2])
                     , gl_fptof(m[3])
                     , gl_fptof(m[4])
                     , gl_fptof(m[5])
                     , gl_fptof(m[6])
                     , gl_fptof(m[7])
                     , gl_fptof(m[8])
                     , gl_fptof(m[9])
                     , gl_fptof(m[10])
                     , gl_fptof(m[11])
                     , gl_fptof(m[12])
                     , gl_fptof(m[13])
                     , gl_fptof(m[14])
                     , gl_fptof(m[15])};

  glMultMatrixf(fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  glOrthof(gl_fptof(left), gl_fptof(right), gl_fptof(bottom), gl_fptof(top), gl_fptof(zNear), gl_fptof(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  glRotatef(gl_fptof(angle), gl_fptof(x), gl_fptof(y), gl_fptof(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  glScalef(gl_fptof(x), gl_fptof(y), gl_fptof(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  glTranslatef(gl_fptof(x), gl_fptof(y), gl_fptof(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w)
{
  glVertex4f(gl_fptof(x), gl_fptof(y), gl_fptof(z), gl_fptof(w));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  glColor4f(gl_fptof(red), gl_fptof(green), gl_fptof(blue), gl_fptof(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
  glTexCoord4f(gl_fptof(s), gl_fptof(t), gl_fptof(r), gl_fptof(q));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  glNormal3f(gl_fptof(nx), gl_fptof(ny), gl_fptof(nz));
}

#endif // CONFIG_FPU
