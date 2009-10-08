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


#ifndef PS2_3DRENDERER_H
#define PS2_3DRENDERER_H


#include "../../../../gl/softGLF.h"
#include "../../../../gl/glMatrix.h"
#include "../../../../gl/context.h"
//#include "../../../../gl/textures.h"
#include "vhl/vector.h"
#include "videoDevice.h"
#include "gif.h"


//-----------------------------------------------------------------------------
class CPS2TextureLevel
{
public:
  CPS2TextureLevel();
  virtual ~CPS2TextureLevel();

  virtual void init();
  virtual void use();
  virtual void free();
  bool used(){return used_;}

public:
  void * data;

  uint16_t ps2Width;
  uint32_t ps2GSAddr;

private:
  bool used_;
};

//-----------------------------------------------------------------------------
class CPS2Texture
 : public CTexture
{
public:
  CPS2Texture(CGIFPacket & packet);
  virtual ~CPS2Texture();

  virtual void free();
  virtual void bind();

public:
  CPS2TextureLevel level_[7];
  uint16_t maxLevel_;

  // Precalculated values
  uint16_t widthBitNr;
  uint16_t heightBitNr;
  uint16_t ps2MinFilter;
  uint16_t ps2MagFilter;
  uint16_t psm_;
  uint16_t rgba_;

private:
  CGIFPacket & packet_;
};

//-----------------------------------------------------------------------------
class CPS23DRenderer
 : public raster::IRasterizer
 , public CAPS2Renderer
{
public:
  CPS23DRenderer(CPS2VideoDevice & device);
  virtual ~CPS23DRenderer();

  // Surfaces
  virtual void       setSurface(CSurface * surface);
  virtual CSurface * getSurface();

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

  // Buffer
  virtual void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void clear(GLbitfield mask);
  virtual void viewport(GLint x, GLint y, GLsizei width, GLsizei height);
  virtual void setUsePixelCenter(bool bCenter);

  // RASTER!
  virtual void rasterTriangle(const raster::SVertex & v0, const raster::SVertex & v1, const raster::SVertex & v2);

  // Flush all triangles (very important for tile based rendering)
  virtual void flush();

private:
  void zbuffer(bool enable);

private:
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
//  CTexture  * pCurrentTex_;
//  CTexture  * textures_[MAX_TEXTURE_COUNT];

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

private:
  CPS2VideoDevice & device_;
  uint16_t    ps2ZPSM_;
  uint32_t    ps2ZBufferAddr_;
  uint16_t    ps2Shading_;
  uint16_t    ps2Textures_;
  uint16_t    ps2Fog_;
  uint16_t    ps2AlphaBlend_;
  uint16_t    ps2Aliasing_;
  uint16_t    ps2DepthFunction_;
  bool        ps2DepthInvert_;
  uint32_t    zMax_;

  CPS2Texture * pCurrentTex_;
  CPS2Texture * textures_[MAX_TEXTURE_COUNT];
};


#endif // PS2_GLESCONTEXT_H
