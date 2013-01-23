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


#ifndef GL_RENDER_H
#define GL_RENDER_H


#include "renderContext.h"
#include "renderMatrix.h"
#include "GL/gl.h"
#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "textures.h"
#include "vhl/vector.h"
#include "vhl/color.h"


//-----------------------------------------------------------------------------
struct SCulling
{
  bool          enabled;
  GLenum        mode;
  GLenum        front;
  bool          cullCW; // Cull Clock-Wise
};

//-----------------------------------------------------------------------------
struct SHints
{
  GLenum        fog;
  GLenum        lineSmooth;
  GLenum        perspectiveCorrection;
  GLenum        pointSmooth;
};

//-----------------------------------------------------------------------------
template <class T>
struct TTexturing
{
  bool          enabled;

  T             coordCurrent[4];
  GLenum        envMode;
  TColor<T>     envColor;
};

//-----------------------------------------------------------------------------
template <class T>
struct TDepthTest
{
  bool          enabled;

  bool          mask;
  GLenum        function;
  T             clear;
  T             rangeNear;
  T             rangeFar;
};

//-----------------------------------------------------------------------------
struct SBlending
{
  bool          enabled;

  GLenum        sourceFactor;
  GLenum        destFactor;
};

//-----------------------------------------------------------------------------
template <class T>
struct TAlphaTest
{
  bool        enabled;

  GLenum      func;
  T           value;
};

//-----------------------------------------------------------------------------
template <class T>
struct TLight
{
  bool          enabled;

  TColor<T>     ambient;
  TColor<T>     diffuse;
  TColor<T>     specular;

  TVector4<T>   position;
  TVector3<T>   positionNormal;
};

//-----------------------------------------------------------------------------
template <class T>
struct TLighting
{
  bool          enabled;
  TLight<T>     light[8];

  bool          normalizeEnabled;
  TVector3<T>   normal;

  bool          materialColorEnabled;
};

//-----------------------------------------------------------------------------
template <class T>
struct TMaterial
{
  TColor<T>   ambient;
  TColor<T>   diffuse;
  TColor<T>   specular;
  TColor<T>   emission;
  T           shininess;
  GLenum      colorMode;
};

//-----------------------------------------------------------------------------
template <class T>
struct TFog
{
  bool        enabled;

  GLint       mode;
  T           density;
  T           start;
  T           end;
  T           linear_scale; // 1 / (end - start)
  TColor<T>   color;
};

//-----------------------------------------------------------------------------
template <class T>
struct TGLState
{
  // Culling
  SCulling        culling;
  // Hints
  SHints          hints;
  // Colors
  TColor<T>       clCurrent;
  TColor<T>       clClear;
  GLenum          shadingModel;
  bool            smoothShading;
  // Textures
  TTexturing<T>   texturing;
  // Depth testing
  TDepthTest<T>   depthTest;
  // Alpha Blending
  SBlending       blending;
  // Alpha testing
  TAlphaTest<T>   alphaTest;
  // Lighting
  TLighting<T>    lighting;
  // Material
  TMaterial<T>    materialFront;
  TMaterial<T>    materialBack;
  // Fog
  TFog<T>         fog;
};


//-----------------------------------------------------------------------------
class CRenderer
 : public virtual I3DRenderer
 , public virtual CAGLFixedToFloat
 , public virtual CAGLErrorHandler
 , public virtual CAGLBuffers
 , public virtual CAGLMatrixFloat
{
public:
  CRenderer();
  virtual ~CRenderer();

  void setRaster(raster::IRasterizer * rast);

  virtual void setSurface(CSurface * surface);

  virtual void glAlphaFunc(GLenum func, GLclampf ref);
  virtual void glBindTexture(GLenum target, GLuint texture);
  virtual void glBlendFunc(GLenum sfactor, GLenum dfactor);
  virtual void glClear(GLbitfield mask);
  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void glClearDepth(GLclampd depth);
  virtual void glColorMaterial(GLenum face, GLenum mode);
  virtual void glColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table);
  virtual void glCullFace(GLenum mode);
  virtual void glDeleteTextures(GLsizei n, const GLuint *textures);
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
  virtual void glGenTextures(GLsizei n, GLuint *textures);
  virtual void glGetFloatv(GLenum pname, GLfloat * params);
  virtual void glHint(GLenum target, GLenum mode);
  virtual void glLightf(GLenum light, GLenum pname, GLfloat param);
  virtual void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
  virtual void glMaterialf(GLenum face, GLenum pname, GLfloat param);
  virtual void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
  virtual void glShadeModel(GLenum mode);
  virtual void glTexEnvf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTexParameterx(GLenum target, GLenum pname, GLfixed param);
  virtual void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

  virtual void glBegin(GLenum mode);
  virtual void glEnd();

  virtual void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  virtual void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);

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

  void interpolateVertex(SVertexF & c, SVertexF & a, SVertexF & b, GLfloat t);

protected:
  raster::IRasterizer * pRaster_;

  TGLState<GLfloat> state_;

  // Primitive assembly
  bool        bInBeginEnd_;
  GLenum      rasterMode_;
  SVertexF    vertices_[3]; // Vertex buffer for primitive assembly
  SVertexF  * triangle_[3]; // Assembled triangle
  bool        bFlipFlop_;   // Triangle strip
  GLint       vertIdx_;     // Current index into vertex buffer

  // Clipping planes
  TVector4<GLfloat> clipPlane_[6];
};


#endif // GL_RENDERER_H
