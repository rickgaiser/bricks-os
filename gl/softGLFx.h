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


#ifndef SOFTGLFX_H
#define SOFTGLFX_H


#include "context.h"
#include "glstate.h"

#include "GL/gl.h"

#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "glMatrix.h"
#include "textures.h"
#include "vhl/fixedPoint.h"
#include "vhl/vector.h"

#ifdef __BRICKS__
#include "asm/arch/memory.h"
#else
#define FAST_CODE
#endif


//-----------------------------------------------------------------------------
class CASoftGLESFixed
 : public virtual CAGLESFloatToFxContext
 , public virtual CAGLESBase
 , public virtual CAGLESMatrixFx
{
public:
  CASoftGLESFixed();
  virtual ~CASoftGLESFixed();

  void setRaster(raster::IRasterizer * rast);

  virtual void setSurface(CSurface * surface);

  virtual void glAlphaFunc(GLenum func, GLclampf ref);
  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glDepthRangex(GLclampx zNear, GLclampx zFar);
  virtual void glDepthFunc(GLenum func);
  virtual void glDepthMask(GLboolean flag);
  virtual void glDisable(GLenum cap);
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count);
  virtual void glEnable(GLenum cap);
  virtual void glFinish(void);
  virtual void glFlush(void);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glMaterialx(GLenum face, GLenum pname, GLfixed param);
  virtual void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params);
  virtual void glShadeModel(GLenum mode);

  virtual void glBegin(GLenum mode);
  virtual void glEnd();
  virtual void glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);

  virtual void glGetFloatv(GLenum pname, GLfloat * params);
  virtual void glBlendFunc(GLenum sfactor, GLenum dfactor);
  virtual void glTexEnvf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params);

  // Textures
  virtual void glBindTexture(GLenum target, GLuint texture);
  virtual void glDeleteTextures(GLsizei n, const GLuint *textures);
  virtual void glGenTextures(GLsizei n, GLuint *textures);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTexParameterx(GLenum target, GLenum pname, GLfixed param);
  virtual void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
  // Rasterization
  virtual void glClear(GLbitfield mask);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  // Vertex shader
  virtual void vertexShaderTransform(SVertexFx & v);
  virtual void vertexShaderLight(SVertexFx & v);

  // Fragment shader
  virtual void fragmentCull(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2);
  virtual void fragmentClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2);

  // Rasterizer
  virtual void primitiveAssembly(SVertexFx & v);

  virtual void rasterTriangleClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2, uint32_t clipBit = 0);
  virtual void rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2);

  void interpolateVertex(SVertexFx & c, SVertexFx & a, SVertexFx & b, CFixed t);

protected:
  raster::IRasterizer * pRaster_;

  TGLState<CFixed> state_;

  bool        bSmoothShading_;

  // Vertex transformations
  CFixed      xA_;
  CFixed      xB_;
  CFixed      yA_;
  CFixed      yB_;
  CFixed      zA_;
  CFixed      zB_;

  // Primitive assembly
  bool        beginValid_;
  GLenum      rasterMode_;
  SVertexFx   vertices[3];  // Vertex buffer for primitive assembly
  SVertexFx * triangle_[3]; // Assembled triangle
  bool        bFlipFlop_;   // Triangle strip
  GLint       vertIdx_;     // Current index into vertex buffer

  // Clipping planes
  TVector4<CFixed> clipPlane_[6];

  // Rasterizer
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;

private:
  void _glDrawArrays(GLenum mode, GLint first, GLsizei count)        FAST_CODE;
  void _vertexShaderTransform(SVertexFx & v)                         FAST_CODE;
  void _vertexShaderLight(SVertexFx & v)                             FAST_CODE;
  void _fragmentCull(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2) FAST_CODE;
  void _fragmentClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2) FAST_CODE;
  void _primitiveAssembly(SVertexFx & v)                             FAST_CODE;
};


#endif // GL_CONTEXT_H
