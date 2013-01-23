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


#include "renderContext.h"
#include "CFuncDebug.h"
#include "vhl/fixedPoint.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CAGLErrorHandler::CAGLErrorHandler()
 : errorCode_(GL_NO_ERROR)
 , bError_(false)
{
  FUNCTION_DEBUG();
}

//-----------------------------------------------------------------------------
CAGLErrorHandler::~CAGLErrorHandler()
{
  FUNCTION_DEBUG();
}

//-----------------------------------------------------------------------------
GLenum
CAGLErrorHandler::glGetError(void)
{
  FUNCTION_DEBUG();

  GLenum err(errorCode_);

  bError_ = false;
  errorCode_ = GL_NO_ERROR;

  return err;
}

//-----------------------------------------------------------------------------
void
CAGLErrorHandler::setError(GLenum error)
{
  FUNCTION_DEBUG();

  if(bError_ == false)
  {
    bError_ = true;
    errorCode_ = error;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CAGLBuffers::CAGLBuffers()
 : bBufColorEnabled_(false)
 , bBufNormalEnabled_(false)
 , bBufTexCoordEnabled_(false)
 , bBufVertexEnabled_(false)
{
  FUNCTION_DEBUG();
}

//-----------------------------------------------------------------------------
CAGLBuffers::~CAGLBuffers()
{
  FUNCTION_DEBUG();
}

//-----------------------------------------------------------------------------
void
CAGLBuffers::glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  FUNCTION_DEBUG();

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
CAGLBuffers::glDisableClientState(GLenum array)
{
  FUNCTION_DEBUG();

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
CAGLBuffers::glEnableClientState(GLenum array)
{
  FUNCTION_DEBUG();

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
CAGLBuffers::glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
  FUNCTION_DEBUG();

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
CAGLBuffers::glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  FUNCTION_DEBUG();

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
CAGLBuffers::glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  FUNCTION_DEBUG();

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
//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  FUNCTION_DEBUG();

  glClearColor(gl_fptof(red), gl_fptof(green), gl_fptof(blue), gl_fptof(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glClearDepthx(GLclampx depth)
{
  FUNCTION_DEBUG();

  glClearDepth(gl_fptof(depth));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glDepthRangex(GLclampx zNear, GLclampx zFar)
{
  FUNCTION_DEBUG();

  glDepthRangef(gl_fptof(zNear), gl_fptof(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glFogx(GLenum pname, GLfixed param)
{
  FUNCTION_DEBUG();

  glFogf(pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glFogxv(GLenum pname, const GLfixed *params)
{
  FUNCTION_DEBUG();

  GLfloat fparams[] = {gl_fptof(params[0])
                     , gl_fptof(params[1])
                     , gl_fptof(params[2])
                     , gl_fptof(params[3])};

  glFogfv(pname, fparams);
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  FUNCTION_DEBUG();

  glFrustumf(gl_fptof(left), gl_fptof(right), gl_fptof(bottom), gl_fptof(top), gl_fptof(zNear), gl_fptof(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glLightx(GLenum light, GLenum pname, GLfixed param)
{
  FUNCTION_DEBUG();

  glLightf(light, pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  FUNCTION_DEBUG();

  GLfloat fparams[] = {gl_fptof(params[0])
                     , gl_fptof(params[1])
                     , gl_fptof(params[2])
                     , gl_fptof(params[3])};

  glLightfv(light, pname, fparams);
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glLoadMatrixx(const GLfixed *m)
{
  FUNCTION_DEBUG();

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
CAGLFixedToFloat::glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
  FUNCTION_DEBUG();

  glMaterialf(face, pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glMaterialxv(GLenum face, GLenum pname, const GLfixed * params)
{
  FUNCTION_DEBUG();

  GLfloat fparams[] = {gl_fptof(params[0])
                     , gl_fptof(params[1])
                     , gl_fptof(params[2])
                     , gl_fptof(params[3])};

  glMaterialfv(face, pname, fparams);
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glMultMatrixx(const GLfixed *m)
{
  FUNCTION_DEBUG();

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
CAGLFixedToFloat::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  FUNCTION_DEBUG();

  glOrthof(gl_fptof(left), gl_fptof(right), gl_fptof(bottom), gl_fptof(top), gl_fptof(zNear), gl_fptof(zFar));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  FUNCTION_DEBUG();

  glRotatef(gl_fptof(angle), gl_fptof(x), gl_fptof(y), gl_fptof(z));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  FUNCTION_DEBUG();

  glScalef(gl_fptof(x), gl_fptof(y), gl_fptof(z));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  FUNCTION_DEBUG();

  glTranslatef(gl_fptof(x), gl_fptof(y), gl_fptof(z));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w)
{
  FUNCTION_DEBUG();

  glVertex4f(gl_fptof(x), gl_fptof(y), gl_fptof(z), gl_fptof(w));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  FUNCTION_DEBUG();

  glColor4f(gl_fptof(red), gl_fptof(green), gl_fptof(blue), gl_fptof(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
  FUNCTION_DEBUG();

  glTexCoord4f(gl_fptof(s), gl_fptof(t), gl_fptof(r), gl_fptof(q));
}

//-----------------------------------------------------------------------------
void
CAGLFixedToFloat::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  FUNCTION_DEBUG();

  glNormal3f(gl_fptof(nx), gl_fptof(ny), gl_fptof(nz));
}
