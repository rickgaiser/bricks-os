#ifndef PS2_GLESCONTEXT_H
#define PS2_GLESCONTEXT_H


#include "../../../../gl/softGLF.h"
#include "../../../../gl/matrix.h"
#include "../../../../gl/context.h"
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
 : public CAGLESFxToFloatContext
 , public CAGLESBuffers
 , public CAGLESCull
 , public CAGLESMatrixF
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
  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void glClearDepthf(GLclampf depth);
  virtual void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  virtual void glDepthRangef(GLclampf zNear, GLclampf zFar);
  virtual void glDepthFunc(GLenum func);
  virtual void glDisable(GLenum cap);
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count);
  virtual void glEnable(GLenum cap);
  virtual void glFinish(void);
  virtual void glFlush(void);
  virtual void glFogf(GLenum pname, GLfloat param);
  virtual void glFogfv(GLenum pname, const GLfloat *params);
  virtual void glLightf(GLenum light, GLenum pname, GLfloat param);
  virtual void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
  virtual void glMaterialf(GLenum face, GLenum pname, GLfloat param);
  virtual void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
  virtual void glShadeModel(GLenum mode);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

  virtual void glBindTexture(GLenum target, GLuint texture);
  virtual void glDeleteTextures(GLsizei n, const GLuint *textures);
  virtual void glGenTextures(GLsizei n, GLuint *textures);
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
  virtual void glTexParameterx(GLenum target, GLenum pname, GLfixed param);

protected:
  virtual void vertexShader(SVertexF & v);

protected:
  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfloat     depthClear_;
  uint32_t    zClearValue_;
  uint32_t  * zbuffer;
  GLclampf    zNear_;
  GLclampf    zFar_;

  GLenum      shadingModel_;

  // Colors
  SColorF     clCurrent;
  SColorF     clClear;

  // Lighting
  bool        lightingEnabled_;
  SLightF     lights_[8];

  // Normals
  bool        normalizeEnabled_;
  GLfloat     normal_[4];

  // Material
  SColorF     matColorAmbient_;
  SColorF     matColorDiffuse_;
  SColorF     matColorSpecular_;
  SColorF     matColorEmission_;
  GLfloat     matShininess_;

  // Fog
  bool        fogEnabled_;
  GLfloat     fogDensity_;
  GLfloat     fogStart_;
  GLfloat     fogEnd_;
  SColorF     fogColor_;

  // Textures
  bool        texturesEnabled_;
  STexturePS2 * pCurrentTex_;
  STexturePS2 textures_[MAX_TEXTURE_COUNT];

  // Viewport
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;

private:
  uint16_t    ps2Shading_;
  uint16_t    ps2Textures_;
  uint16_t    ps2Fog_;
  uint16_t    ps2AlphaBlend_;
  uint16_t    ps2Aliasing_;
  uint16_t    ps2DepthFunction_;
  bool        ps2DepthInvert_;
  uint32_t    ps2ZMax_;
};


#endif // PS2_GLESCONTEXT_H
