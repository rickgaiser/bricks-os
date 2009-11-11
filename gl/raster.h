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


#ifndef GL_RASTER_H
#define GL_RASTER_H


#include "GL/gl.h"
#include "vhl/color.h"
#include "kernel/videoManager.h"


namespace raster
{


//-----------------------------------------------------------------------------
// OLD: Too much data in there
template <class T>
struct TGradient
{
  T ddx;          // This needs to be added when moving 1 pixel in x direction
  T ddy;          // This needs to be added when moving 1 pixel in y direction
  T start;        // Value at x=0, y=0
  T ddy_left;     // This needs to be added when moving 1 pixel in y direction, and moving x by the left scanline

  T current;      // Current value of the current pixel
  T current_left; // Current value of the left edge
};

//-----------------------------------------------------------------------------
// Gradient for scanline interpolation
template <class T>
struct TGradientScanline
{
  T ddx;          // This needs to be added when moving 1 pixel in x direction
  T ddy;          // This needs to be added when moving 1 pixel in y direction
  T ddy_left;     // This needs to be added when moving 1 pixel in y direction, and moving x by the left scanline
};

//-----------------------------------------------------------------------------
// Gradient for block interpolation
template <class T>
struct TGradientBlock
{
  T ddx;          // This needs to be added when moving 1 pixel in x direction
  T ddy;          // This needs to be added when moving 1 pixel in y direction
  T start;        // Value at x=0, y=0
};
template <class T>
struct TGradientBlockInterpolator
{
  T current;      // Current value of the current pixel
  T current_left; // Current value of the left edge
};

//-----------------------------------------------------------------------------
template <class T>
struct TColorFx
{
  inline void operator+=(const TColorFx & c)
  {
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
  }

  T r;
  T g;
  T b;
  T a;
};
typedef TColorFx<int32_t> SColor;
//-----------------------------------------------------------------------------
template <class T>
struct TColorF
{
  inline void operator+=(const TColorF & c)
  {
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
  }

  inline void operator*=(const TColorF & c)
  {
    r *= c.r;
    g *= c.g;
    b *= c.b;
    a *= c.a;
  }

  T r;
  T g;
  T b;
  T a;
};
typedef TColorF<GLfloat> SColorF;

//-----------------------------------------------------------------------------
template <class T>
struct TTexCoord
{
  inline void operator+=(const TTexCoord & t)
  {
    u += t.u;
    v += t.v;
  }

  T u;
  T v;
};
typedef TTexCoord<int32_t> STexCoord;
typedef TTexCoord<GLfloat> STexCoordF;

//-----------------------------------------------------------------------------
#define SHIFT_XY       4
#define DEPTH_Z       14 // Resolution of depth buffer
#define SHIFT_Z       15 // Added resolution for interpolation
#define SHIFT_COLOR   12
struct SVertex
{
  int32_t            x;
  int32_t            y;
  int32_t            z;
  float              w;

  TColorFx<int32_t>  c;
  TTexCoord<float>   t;
};

//-----------------------------------------------------------------------------
class IRasterizer
 : public IRenderer
{
public:
  virtual ~IRasterizer(){}

  // Enabling options
  virtual void enableDepthTest(bool enable) = 0;
  virtual void enableSmoothShading(bool enable) = 0;
  virtual void enableTextures(bool enable) = 0;
  virtual void enableBlending(bool enable) = 0;
  virtual void enableAlphaTest(bool enable) = 0;

  // Depth testing
  virtual void clearDepthf(GLclampf depth) = 0;
  virtual void depthFunc(GLenum func) = 0;
  virtual void depthMask(GLboolean flag) = 0;

  // Textures
  virtual void bindTexture(GLenum target, GLuint texture) = 0;
  virtual void deleteTextures(GLsizei n, const GLuint * textures) = 0;
  virtual void genTextures(GLsizei n, GLuint * textures) = 0;
  virtual void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels) = 0;
  virtual void texParameterf(GLenum target, GLenum pname, GLfloat param) = 0;
  virtual void texEnvf(GLenum target, GLenum pname, GLfloat param) = 0;
  virtual void colorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table) = 0;

  // Blending
  virtual void alphaFunc(GLenum func, GLclampf ref) = 0;
  virtual void blendFunc(GLenum sfactor, GLenum dfactor) = 0;

  // Buffer
  virtual void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;
  virtual void clear(GLbitfield mask) = 0;
  virtual void viewport(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual void setUsePixelCenter(bool bCenter) = 0;

  // RASTER!
  virtual void begin(GLenum mode) = 0;
  virtual void end() = 0;
  virtual void rasterTriangle(const SVertex & v0, const SVertex & v1, const SVertex & v2) = 0;

  // Flush all triangles (very important for tile based rendering)
  virtual void flush() = 0;
};


};


#endif // GL_RASTER_H
