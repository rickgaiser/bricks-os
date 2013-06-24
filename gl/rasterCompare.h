/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef GL_RASTERCOMPARE_H
#define GL_RASTERCOMPARE_H


#include "GL/gl.h"
#include "raster.h"
#include "rasterCommon.h"


namespace raster
{


//-----------------------------------------------------------------------------
#define COLOR_BLACK   0x00000000
#define COLOR_GRAY    0x00777777
#define COLOR_WHITE   0x00ffffff
#define COLOR_L_RED   0x00770000
#define COLOR_H_RED   0x00ff0000
#define COLOR_L_GREEN 0x00007700
#define COLOR_H_GREEN 0x0000ff00
#define COLOR_L_BLUE  0x00000077
#define COLOR_H_BLUE  0x000000ff
//-----------------------------------------------------------------------------
inline void
testAndSetPixel(uint32_t & pixel)
{
  switch(pixel)
  {
    case COLOR_BLACK:
      pixel = COLOR_WHITE;  // OK
      break;
    case COLOR_WHITE:
      pixel = COLOR_L_RED;  // ERROR
      break;
    case COLOR_L_RED:
    case COLOR_GRAY:
      ;                     // Leave it
      break;
    default:
      pixel = COLOR_GRAY;   // Unknown "gray" area
      break;
  };
}

//-----------------------------------------------------------------------------
class CRasterizerCompare
 : public IRasterizer
{
public:
  CRasterizerCompare(IRasterizer * pTest = NULL, IRasterizer * pRef = NULL);
  virtual ~CRasterizerCompare();

  void setSurface(CSurface * surface);

  void compareRasterTest(IRasterizer * pTest);
  void compareRasterRef(IRasterizer * pRef);

  // Enabling options
  void enableDepthTest(bool enable);
  void enableSmoothShading(bool enable);
  void enableTextures(bool enable);
  void enableBlending(bool enable);
  void enableAlphaTest(bool enable);

  // Depth testing
  void clearDepthf(GLclampf depth);
  void depthRange(GLclampf zNear, GLclampf zFar);
  void depthFunc(GLenum func);
  void depthMask(GLboolean flag);

  // Textures
  void bindTexture(GLenum target, GLuint texture);
  void deleteTextures(GLsizei n, const GLuint * textures);
  void genTextures(GLsizei n, GLuint * textures);
  void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
  void texParameterf(GLenum target, GLenum pname, GLfloat param);
  void texEnvf(GLenum target, GLenum pname, GLfloat param);
  void texEnvfv(GLenum target, GLenum pname, const GLfloat * params);
  void colorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table);

  // Blending
  void alphaFunc(GLenum func, GLclampf ref);
  void blendFunc(GLenum sfactor, GLenum dfactor);

  // Buffer
  void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  void clear(GLbitfield mask);
  void viewport(GLint x, GLint y, GLsizei width, GLsizei height);
  void setUsePixelCenter(bool bCenter);

  // RASTER!
  void begin(GLenum mode);
  void end();
  void rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2);

  // Flush all triangles (very important for tile based rendering)
  void flush();

private:
  IRasterizer * pTest_;
  IRasterizer * pRef_;
  bool bError_;
};


};


#endif // GL_RASTERCOMPARE_H
