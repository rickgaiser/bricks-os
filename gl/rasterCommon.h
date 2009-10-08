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


#ifndef GL_RASTERCOMMON_H
#define GL_RASTERCOMMON_H


#include "GL/gl.h"
#include "raster.h"
#include "textures.h"
#ifdef ENABLE_PROFILING
#include "prof/prof.h"
#endif


#define MAX_TEXTURE_COUNT 1024


#ifdef ENABLE_PROFILING
extern CTimeProfiler prof_clear;
extern CTimeProfiler prof_init;
extern CTimeProfiler prof_raster;
extern uint64_t fillRate;
#endif


namespace raster
{


//-----------------------------------------------------------------------------
#define i_mul_shr_64(a,b,shr)       ((int32_t)(((int64_t)(a)*(int64_t)(b)             )>>shr))
#define i_mul_round_shr_64(a,b,shr) ((int32_t)(((int64_t)(a)*(int64_t)(b)+(1<<(shr-1)))>>shr))
#define i_mul_shr(a,b,shr)          ((                   (a)*         (b)             )>>shr)
#define i_mul_round_shr(a,b,shr)    ((                   (a)*         (b)+(1<<(shr-1)))>>shr)

//-----------------------------------------------------------------------------
enum EFastBlendMode
{
  FB_OTHER,
  FB_SOURCE,
  FB_DEST,
  FB_BLEND,
};

//-----------------------------------------------------------------------------
inline void
FloorDivMod(int32_t Numerator, int32_t Denominator, int32_t &Floor, int32_t &Mod)
{
  if(Numerator >= 0)
  {
    // positive case, C is okay
    Floor = Numerator / Denominator;
    Mod   = Numerator % Denominator;
  }
  else
  {
    // Numerator is negative, do the right thing
    Floor = -((-Numerator) / Denominator);
    Mod   =   (-Numerator) % Denominator;
    if(Mod)
    {
      // there is a remainder
      Floor--; Mod = Denominator - Mod;
    }
  }
}

//-----------------------------------------------------------------------------
class CASoftRasterizer
 : public IRasterizer
{
public:
  CASoftRasterizer();
  virtual ~CASoftRasterizer();

  // Enabling options
  virtual void enableDepthTest(bool enable);
  virtual void enableSmoothShading(bool enable);
  virtual void enableTextures(bool enable);
  virtual void enableBlending(bool enable);

  // Depth testing
  virtual void clearDepthf(GLclampf depth);
  virtual void depthRangef(GLclampf zNear, GLclampf zFar);
  virtual void depthFunc(GLenum func);

  // Textures
  virtual void bindTexture(GLenum target, GLuint texture);
  virtual void deleteTextures(GLsizei n, const GLuint * textures);
  virtual void genTextures(GLsizei n, GLuint * textures);
  virtual void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
  virtual void texParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void texEnvf(GLenum target, GLenum pname, GLfloat param);

  // Blending
  virtual void blendFunc(GLenum sfactor, GLenum dfactor);

  virtual void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void clear(GLbitfield mask);
  virtual void viewport(GLint x, GLint y, GLsizei width, GLsizei height);
  virtual void setUsePixelCenter(bool bCenter);

  virtual void rasterTriangle(const SVertex & v0, const SVertex & v1, const SVertex & v2) = 0;
  virtual void flush();

protected:
  bool rasterDepth(uint32_t z_pix, uint32_t z_buf);

protected:
  SColorF     clClear;
  int32_t   * pZBuffer_;

  // Shading model
  bool        bSmoothShadingEnabled_;

  // Depth testing
  bool        bDepthTestEnabled_;
  GLenum      depthFunction_;
  GLfloat     depthClear_;
  int32_t     zClearValue_;
  GLclampf    zRangeNear_;
  GLclampf    zRangeFar_;

  // Textures
  bool        bTexturesEnabled_;
  GLenum      texEnvMode_;
  SColorF     texEnvColor_;
  CTexture  * pCurrentTex_;
  CTexture  * textures_[MAX_TEXTURE_COUNT];

  // Blending
  bool        bBlendingEnabled_;
  GLenum      blendSFactor_;
  GLenum      blendDFactor_;
  EFastBlendMode blendFast_;

  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;
  GLsizei     viewportPixelCount;

  // Pixel center
  bool        bUsePixelCenter_;
  int32_t     pixelFloorOffset_;
  int32_t     pixelCenterOffset_;
  int32_t     oneMinusPixelCenterOffset_;
};


};


#endif // GL_RASTERCOMMON_H
