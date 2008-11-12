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

#include "GL/gl.h"

#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "glMatrix.h"
#include "textures.h"
#include "edge.h"
#include "vhl/fixedPoint.h"
#include "vhl/vector.h"

#include "asm/arch/memory.h"


#define fpRGB(r,g,b) \
  (0x8000 | \
  (((b.value*255) >>  9) & 0x7c00) | \
  (((g.value*255) >> 14) & 0x03e0) | \
  (((r.value*255) >> 19) & 0x001f))


//-----------------------------------------------------------------------------
class CASoftGLESFixed
 : public virtual CAGLESFloatToFxContext
 , public virtual CAGLESBase
 , public virtual CAGLESMatrixFx
{
public:
  CASoftGLESFixed();
  virtual ~CASoftGLESFixed();

  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glDepthRangex(GLclampx zNear, GLclampx zFar);
  virtual void glDepthFunc(GLenum func);
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
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

  virtual void glBegin(GLenum mode);
  virtual void glEnd();
  virtual void glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);

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
  virtual void rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2) = 0;

  void interpolateVertex(SVertexFx & vNew, SVertexFx & vOld, SVertexFx & vFrom, CFixed t);

  virtual void zbuffer(bool enable) = 0;

protected:
  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfixed     depthClear_;
  uint16_t    zClearValue_;
  GLfixed     zLoss_;
  CFixed      zNear_;
  CFixed      zFar_;
  CFixed      zA_;
  CFixed      zB_;
  uint32_t    zMax_;

  GLenum      shadingModel_;

  // Colors
  SColorFx    clCurrent;
  SColorFx    clClear;

  // Lighting
  bool        lightingEnabled_;
  SLightFx    lights_[8];

  // Normals
  bool        normalizeEnabled_;
  TVector3<CFixed> normal_;

  // Material
  SColorFx    matColorAmbient_;
  SColorFx    matColorDiffuse_;
  SColorFx    matColorSpecular_;
  SColorFx    matColorEmission_;
  CFixed      matShininess_;

  // Fog
  bool        fogEnabled_;
  GLfixed     fogDensity_;
  GLfixed     fogStart_;
  GLfixed     fogEnd_;
  SColorFx    fogColor_;

  // Textures
  bool        texturesEnabled_;
  CFixed      texCoordCurrent_[4];

  // Vertex transformations
  CFixed      xA_;
  CFixed      xB_;
  CFixed      yA_;
  CFixed      yB_;

  // Primitive assembly
  GLenum      rasterMode_;
  SVertexFx   vertices[3];  // Vertex buffer for primitive assembly
  SVertexFx * triangle_[3]; // Assembled triangle
  bool        bFlipFlop_;   // Triangle strip
  GLint       vertIdx_;     // Current index into vertex buffer

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

//-----------------------------------------------------------------------------
class CSoftGLESFixed
 : public CASoftGLESFixed
 , public CAGLESTextures
{
public:
  CSoftGLESFixed();
  virtual ~CSoftGLESFixed();

  virtual void glClear(GLbitfield mask);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  virtual void rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2);
  virtual void zbuffer(bool enable);

protected:
  uint16_t  * pZBuffer_;
  CEdgeFx   * edge1;
  CEdgeFx   * edge2;

private:
  void _rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2) FAST_CODE;

  bool testAndSetDepth(GLfixed z, uint32_t index)                      FAST_CODE;
  void hline   (CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c)     FAST_CODE;
  void hlineZ  (CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c)     FAST_CODE;
  void hlineC  (CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
  void hlineZC (CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
  void hlineTa (CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
  void hlineZTa(CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
//  void hlineZTp(CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
};


#include "softGLFx.inl"


#endif // GL_CONTEXT_H
