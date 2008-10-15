#ifndef PS2_GLESCONTEXT_H
#define PS2_GLESCONTEXT_H


#include "../../../../gl/softGLF.h"
#include "../../../../gl/glMatrix.h"
#include "../../../../gl/context.h"
#include "../../../../gl/textures.h"
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

  virtual void init();
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
class CPS2GLESContext
 : public CASoftGLESFloat
 , public CAGLESTextures
 , public CAPS2Renderer
{
public:
  CPS2GLESContext(CPS2VideoDevice & device);
  virtual ~CPS2GLESContext();

  // Surfaces
  virtual void       setSurface(CSurface * surface){CAPS2Renderer::setSurface(surface);}
  virtual CSurface * getSurface()                  {return CAPS2Renderer::getSurface();}

  // Flush operations to surface
  virtual void       flush()                       {CAPS2Renderer::flush();}

  virtual void glClear(GLbitfield mask);
  virtual void glDepthFunc(GLenum func);
  virtual void glDisable(GLenum cap);
  virtual void glEnable(GLenum cap);
  virtual void glShadeModel(GLenum mode);

//  virtual void glBindTexture(GLenum target, GLuint texture);
//  virtual void glDeleteTextures(GLsizei n, const GLuint *textures);
//  virtual void glGenTextures(GLsizei n, GLuint *textures);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param);

  virtual void glBegin(GLenum mode);
  virtual void glEnd();

protected:
  virtual void rasterTriangleClip(SVertexF & v0, SVertexF & v1, SVertexF & v2, uint32_t clipBit = 0);
  virtual void rasterTriangle(SVertexF & v0, SVertexF & v1, SVertexF & v2);
  virtual void zbuffer(bool enable);
  virtual CTexture * getTexture();

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
};


#endif // PS2_GLESCONTEXT_H
