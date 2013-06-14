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


#include "../../../../gl/raster.h"
#include "GL/gl.h"
#include "vhl/vector.h"
#include "videoDevice.h"
#include "gif.h"


#define PS2_MAX_TEXTURE_COUNT 1024


//-----------------------------------------------------------------------------
class CPS2TextureLevel
{
public:
  CPS2TextureLevel();
  ~CPS2TextureLevel();

  void bind();
  void use();
  void free();
  bool used();

public:
  void * data;

  unsigned int gsMemLocation_;
  unsigned int ps2Width_;
  unsigned int iWidth_;
  unsigned int iHeight_;
  unsigned int iPsm_;

private:
  bool used_;
};

//-----------------------------------------------------------------------------
class CPS2Texture
{
public:
  CPS2Texture(CGIFPacket & packet);
  ~CPS2Texture();

  void free();
  void bind(uint16_t envMode);

public:
  CPS2TextureLevel level_[7];
  uint16_t maxLevel_;

  int32_t width;
  int32_t height;
  GLint minFilter;
  GLint magFilter;
  GLint uWrapMode;
  GLint vWrapMode;

  // Precalculated values
  uint16_t iWidthLog2_;
  uint16_t iHeightLog2_;
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
  virtual void enableAlphaTest(bool enable);

  // Depth testing
  virtual void clearDepthf(GLclampf depth);
  virtual void depthRange(GLclampf zNear, GLclampf zFar);
  virtual void depthFunc(GLenum func);
  virtual void depthMask(GLboolean flag);

  // Textures
  virtual void bindTexture(GLenum target, GLuint texture);
  virtual void deleteTextures(GLsizei n, const GLuint * textures);
  virtual void genTextures(GLsizei n, GLuint * textures);
  virtual void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
  virtual void texParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void texEnvf(GLenum target, GLenum pname, GLfloat param);
  virtual void texEnvfv(GLenum target, GLenum pname, const GLfloat * params);
  virtual void colorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table);

  // Blending
  virtual void alphaFunc(GLenum func, GLclampf ref);
  virtual void blendFunc(GLenum sfactor, GLenum dfactor);

  // Buffer
  virtual void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void clear(GLbitfield mask);
  virtual void viewport(GLint x, GLint y, GLsizei width, GLsizei height);
  virtual void setUsePixelCenter(bool bCenter);

  // RASTER!
  virtual void begin(GLenum mode);
  virtual void end();
  virtual void rasterTriangle(const SVertexF & v0, const SVertexF & v1, const SVertexF & v2);

  // Flush all triangles (very important for tile based rendering)
  virtual void flush();

private:
  void initializeGS();
  void bindTexture(CPS2Texture * texture);

private:
  CPS2VideoDevice & device_;

  CFloat_4    clClear;

  // Depth testing
  bool        bDepthTestEnabled_;
  uint16_t    ps2ZPSM_;
  uint32_t    zMax_;
  GLfloat     depthClear_;
  uint32_t    zClearValue_;
  uint32_t    ps2ZBufferAddr_;
  uint16_t    ps2DepthFunction_;
  bool        ps2DepthInvert_;
  bool        bDepthMask_;
  GLclampf    zNear_;
  GLclampf    zFar_;

  // Shading model
  bool        bSmoothShadingEnabled_;

  // Textures
  bool        bTexturesEnabled_;
  GLenum      texEnvMode_;
  bool        bTextureColor_;
  bool        bForceWhiteColor_;
  CPS2Texture * pCurrentTex_;
  CPS2Texture * textures_[PS2_MAX_TEXTURE_COUNT];

  // Alpha testing
  bool        bAlphaTestEnabled_;
  GLenum      alphaFunc_;
  uint16_t    ps2AlphaFunc_;
  GLclampf    alphaValue_;
  uint8_t     ps2AlphaValue_;

  // Fog
  uint16_t    ps2Fog_;

  // Anti aliasing
  uint16_t    ps2Aliasing_;

  // Blending
  bool        bBlendingEnabled_;
  GLenum      blendSFactor_;
  GLenum      blendDFactor_;

  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;
  
  // Vertex transformations
  GLfloat     fXA_;
  GLfloat     fXB_;
  GLfloat     fYA_;
  GLfloat     fYB_;
  GLfloat     fZA_;
  GLfloat     fZB_;
};


#endif // PS2_GLESCONTEXT_H
