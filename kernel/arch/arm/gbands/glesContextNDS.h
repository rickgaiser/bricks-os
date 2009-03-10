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


#ifndef NDS_GLESCONTEXTNDS_H
#define NDS_GLESCONTEXTNDS_H


#include "../../../../gl/softGLFx.h"
#include "../../../../gl/textures.h"
#include "vhl/fixedPoint.h"


#define NDS_MATRIX_PRECISION   12
#define nds_fpfromi(i)   fpfromi(NDS_MATRIX_PRECISION,i)
#define nds_fptoi(i)     fptoi(NDS_MATRIX_PRECISION,i)
#define nds_fpfromf(i)   fpfromf(NDS_MATRIX_PRECISION,i)
#define nds_fptof(i)     fptof(NDS_MATRIX_PRECISION,i)
#define nds_fpmul(i1,i2) fpmul32(NDS_MATRIX_PRECISION,i1,i2)
#define nds_fpdiv(i1,i2) fpdiv32(NDS_MATRIX_PRECISION,i1,i2)

#define gl_to_ndsv(i)    ((i)>>4)  // Vertex  format  4.12
#define gl_to_ndst(i)    ((i)>>12) // Texture format 12.4
#define gl_to_ndsn(i)    (((i) >= gl_fpfromi(1)) ? 0x1ff : ((i) >> 7))  // Normal  format  1.9
#define ndsv_to_gl(i)    ((i)<<4)
#define ndst_to_gl(i)    ((i)<<12)
#define ndsn_to_gl(i)    ((i)<<7)


//-----------------------------------------------------------------------------
typedef GLfixed NDSfixed;

//-----------------------------------------------------------------------------
class CNDSTexture
 : public CTexture
{
public:
  CNDSTexture();
  virtual ~CNDSTexture();

  virtual void bind();

public:
  uint32_t format;
};

//-----------------------------------------------------------------------------
class CNDSGLESContext
 : public CASoftGLESFixed
 , public CAGLESTextures
{
public:
  CNDSGLESContext();
  virtual ~CNDSGLESContext();

  // Surfaces
  virtual void       setSurface(CSurface * surface){IRenderer::setSurface(surface);}
  virtual CSurface * getSurface()                  {return IRenderer::getSurface();}

  // Flush operations to surface
  virtual void       flush()                       {IRenderer::flush();}

  virtual void glClear(GLbitfield mask);
  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glCullFace(GLenum mode);
  virtual void glDisable(GLenum cap);
  virtual void glEnable(GLenum cap);
  virtual void glFlush(void);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glMaterialx(GLenum face, GLenum pname, GLfixed param);
  virtual void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params);
  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glLoadIdentity(void);
  virtual void glLoadMatrixx(const GLfixed *m);
  virtual void glMatrixMode(GLenum mode);
  virtual void glMultMatrixx(const GLfixed *m);
  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

  virtual void glBegin(GLenum mode);
  virtual void glEnd();

protected:
  virtual void vertexShaderTransform(SVertexFx & v);

  virtual void rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2);
  virtual void zbuffer(bool enable);
  virtual CTexture * getTexture();

private:
  void plotPoly(SVertexFx * vtx[3]);
  void updateLights();
  void updateFog();

  uint32_t    iNDSGFXControl_;
  uint32_t    iNDSPolyFormat_;

  GLenum      matrixMode_;
  uint32_t    ndsCurrentMatrixId_;

  static bool        bInitialized_;
  static NDSfixed    fpSin_[];
  static NDSfixed    fpCos_[];

  // NDS Material colors
  uint16_t ndsMatColorAmbient_;
  uint16_t ndsMatColorDiffuse_;
  uint16_t ndsMatColorSpecular_;
  uint16_t ndsMatColorEmission_;
  uint32_t ndsMatShinyness_[128/4];
};


#endif // NDS_GLESCONTEXTNDS_H
