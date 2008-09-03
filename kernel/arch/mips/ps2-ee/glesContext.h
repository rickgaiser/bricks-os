#ifndef PS2_GLESCONTEXT_H
#define PS2_GLESCONTEXT_H


#include "../../../../gl/softGLF.h"
#include "../../../../gl/glMatrix.h"
#include "../../../../gl/context.h"
#include "../../../../gl/vector.h"
#include "videoDevice.h"
#include "gif.h"


#define MAX_TEXTURE_COUNT 32


//-----------------------------------------------------------------------------
struct STexturePS2
{
  bool used;

  GLsizei width;
  GLsizei height;

  GLint texMinFilter;
  GLint texMagFilter;
  GLint texWrapS;
  GLint texWrapT;

  const void * data;
};

//-----------------------------------------------------------------------------
class CPS2GLESContext
 : public CASoftGLESFloat
 , public CAPS2Renderer
{
public:
  CPS2GLESContext();
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

  virtual void glBindTexture(GLenum target, GLuint texture);
  virtual void glDeleteTextures(GLsizei n, const GLuint *textures);
  virtual void glGenTextures(GLsizei n, GLuint *textures);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTexParameterx(GLenum target, GLenum pname, GLfixed param);

  virtual void glBegin(GLenum mode);
  virtual void glEnd();

protected:
  virtual void rasterTriangle(SVertexF & v0, SVertexF & v1, SVertexF & v2);
  virtual void zbuffer(bool enable);

private:
  STexturePS2 * pCurrentTex_;
  STexturePS2 textures_[MAX_TEXTURE_COUNT];
  uint16_t    ps2Shading_;
  uint16_t    ps2Textures_;
  uint16_t    ps2Fog_;
  uint16_t    ps2AlphaBlend_;
  uint16_t    ps2Aliasing_;
  uint16_t    ps2DepthFunction_;
  bool        ps2DepthInvert_;
};


#endif // PS2_GLESCONTEXT_H
