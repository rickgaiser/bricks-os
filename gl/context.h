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


#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H


#include "GL/gl.h"
#ifndef __BRICKS__
#define GL_FIXED 0x140c
#endif
#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "vhl/fixedPoint.h"
#include "vhl/vector.h"
#include "vhl/color.h"


//-----------------------------------------------------------------------------
template <class T>
struct TVertex
{
  // Vertices
  TVector4<T> vo; // Object Coordinates
  TVector4<T> ve; // Eye Coordinates
  TVector4<T> vc; // Clip Coordinates
  TVector4<T> vd; // Normalized Device Coordinates

  // Normals
  TVector3<T> no; // Object Coordinates
  TVector3<T> ne; // Eye Coordinates

  // Color
  TColor<T> c;    // Color
  // Texture coordinates
  T t[2];

  // Window coordinates
  GLint  sx, sy;

  // Depth (on screen)
  uint32_t sz;

  // Clipping flags
  uint32_t clip;

  // Processed by post vertex shader
  bool processed;
};
typedef TVertex<GLfloat> SVertexF;

//-----------------------------------------------------------------------------
struct SBufferPointer
{
  GLint size;
  GLenum type;
  GLsizei stride;
  const GLvoid * pointer;
};

//-----------------------------------------------------------------------------
class IGLErrorHandler
{
public:
  virtual ~IGLErrorHandler(){}

  virtual void setError(GLenum error) = 0;
};

//-----------------------------------------------------------------------------
class CAGLErrorHandler
 : public virtual I3DRenderer
 , public IGLErrorHandler
{
public:
  CAGLErrorHandler();
  virtual ~CAGLErrorHandler();

  virtual GLenum glGetError(void);
  virtual void setError(GLenum error);

private:
  GLenum      errorCode_;
  bool        bError_;
};

//-----------------------------------------------------------------------------
class CAGLBuffers
 : public virtual I3DRenderer
 , public virtual CAGLErrorHandler
{
public:
  CAGLBuffers();
  virtual ~CAGLBuffers();

  virtual void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glDisableClientState(GLenum array);
  virtual void glEnableClientState(GLenum array);
  virtual void glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  virtual void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);

protected:
  // Buffers
  bool bBufColorEnabled_;
  bool bBufNormalEnabled_;
  bool bBufTexCoordEnabled_;
  bool bBufVertexEnabled_;
  SBufferPointer bufColor_;
  SBufferPointer bufNormal_;
  SBufferPointer bufTexCoord_;
  SBufferPointer bufVertex_;
};

//-----------------------------------------------------------------------------
class CAGLFixedToFloat
 : public virtual I3DRenderer
{
public:
  CAGLFixedToFloat(){}
  virtual ~CAGLFixedToFloat(){}

  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glDepthRangex(GLclampx zNear, GLclampx zFar);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glLoadMatrixx(const GLfixed *m);
  virtual void glMaterialx(GLenum face, GLenum pname, GLfixed param);
  virtual void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params);
  virtual void glMultMatrixx(const GLfixed *m);
  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);

  virtual void glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
};


#endif // GL_CONTEXT_H
