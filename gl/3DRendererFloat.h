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


#ifndef GL_3DRENDERERFLOAT_H
#define GL_3DRENDERERFLOAT_H


#include "context.h"
#include "glstate.h"

#include "GL/gl.h"

#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "glMatrix.h"
#include "textures.h"
#include "vhl/vector.h"

#ifdef __BRICKS__
#include "asm/arch/memory.h"
#else
#define FAST_CODE
#endif


//-----------------------------------------------------------------------------
class CSoft3DRendererFloat
 : public virtual I3DRenderer
 , public virtual CAGLFixedToFloat
 , public virtual CAGLErrorHandler
 , public virtual CAGLBuffers
 , public virtual CAGLMatrixFloat
{
public:
  CSoft3DRendererFloat();
  virtual ~CSoft3DRendererFloat();

  void setRaster(raster::IRasterizer * rast);

  virtual void setSurface(CSurface * surface);

  virtual void glAlphaFunc(GLenum func, GLclampf ref);
  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void glClearDepthf(GLclampf depth);
  virtual void glColorMaterial(GLenum face, GLenum mode);
  virtual void glCullFace(GLenum mode);
  virtual void glDepthRangef(GLclampf zNear, GLclampf zFar);
  virtual void glDepthFunc(GLenum func);
  virtual void glDepthMask(GLboolean flag);
  virtual void glDisable(GLenum cap);
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count);
  virtual void glEnable(GLenum cap);
  virtual void glFinish(void);
  virtual void glFlush(void);
  virtual void glFogf(GLenum pname, GLfloat param);
  virtual void glFogfv(GLenum pname, const GLfloat *params);
  virtual void glFrontFace(GLenum mode);
  virtual void glHint(GLenum target, GLenum mode);
  virtual void glLightf(GLenum light, GLenum pname, GLfloat param);
  virtual void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
  virtual void glMaterialf(GLenum face, GLenum pname, GLfloat param);
  virtual void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
  virtual void glShadeModel(GLenum mode);

  virtual void glBegin(GLenum mode);
  virtual void glEnd();
  virtual void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  virtual void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);

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
  virtual void vertexShaderTransform(SVertexF & v);
  virtual void vertexShaderLight(SVertexF & v);

  // Fragment shader
  virtual void fragmentCull(SVertexF & v0, SVertexF & v1, SVertexF & v2);
  virtual void fragmentClip(SVertexF & v0, SVertexF & v1, SVertexF & v2);

  // Rasterizer
  virtual void primitiveAssembly(SVertexF & v);

  virtual void rasterTriangleClip(SVertexF & v0, SVertexF & v1, SVertexF & v2, uint32_t clipBit = 0);
  virtual void rasterTriangle(SVertexF & v0, SVertexF & v1, SVertexF & v2);

  void interpolateVertex(SVertexF & c, SVertexF & a, SVertexF & b, GLfloat t);

protected:
  raster::IRasterizer * pRaster_;

  TGLState<GLfloat> state_;

  // Vertex transformations
  GLfloat     xA_;
  GLfloat     xB_;
  GLfloat     yA_;
  GLfloat     yB_;
  GLfloat     zA_;
  GLfloat     zB_;

  // Primitive assembly
  bool        beginValid_;
  GLenum      rasterMode_;
  SVertexF    vertices[3];  // Vertex buffer for primitive assembly
  SVertexF  * triangle_[3]; // Assembled triangle
  bool        bFlipFlop_;   // Triangle strip
  GLint       vertIdx_;     // Current index into vertex buffer

  // Clipping planes
  TVector4<GLfloat> clipPlane_[6];

  // Rasterizer
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;

private:
  void _glDrawArrays(GLenum mode, GLint first, GLsizei count)     FAST_CODE;
  void _vertexShaderTransform(SVertexF & v)                       FAST_CODE;
  void _vertexShaderLight(SVertexF & v)                           FAST_CODE;
  void _fragmentCull(SVertexF & v0, SVertexF & v1, SVertexF & v2) FAST_CODE;
  void _fragmentClip(SVertexF & v0, SVertexF & v1, SVertexF & v2) FAST_CODE;
  void _primitiveAssembly(SVertexF & v)                           FAST_CODE;
};


#endif // GL_3DRENDERERFLOAT_H
