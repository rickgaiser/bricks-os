#ifndef PS2_GLESCONTEXT_H
#define PS2_GLESCONTEXT_H


#include "../../../../gl/softGLF.h"
#include "../../../../gl/matrix.h"


//-----------------------------------------------------------------------------
class CPS2GLESContext
 : public CAGLESFxToFloatContext
 , public CAGLESBuffers
 , public CAGLESMatrixF
{
public:
  CPS2GLESContext();
  virtual ~CPS2GLESContext();

  virtual void setSurface(CSurface * surface);

  virtual void glClear(GLbitfield mask);
  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void glClearDepthf(GLclampf depth);
  virtual void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  virtual void glCullFace(GLenum mode);
  virtual void glDepthFunc(GLenum func);
  virtual void glDisable(GLenum cap);
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count);
  virtual void glEnable(GLenum cap);
  virtual void glFlush(void);
  virtual void glFogf(GLenum pname, GLfloat param);
  virtual void glFogfv(GLenum pname, const GLfloat *params);
  virtual void glLightf(GLenum light, GLenum pname, GLfloat param);
  virtual void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
  virtual void glShadeModel(GLenum mode);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  uint16_t    ps2Shading_;
  uint16_t    ps2Textures_;
  uint16_t    ps2Fog_;
  uint16_t    ps2AlphaBlend_;
  uint16_t    ps2Aliasing_;
  uint16_t    ps2DepthFunction_;
  bool        ps2DepthInvert_;
  uint32_t    ps2ZMax_;

  CSurface  * renderSurface;
  GLint       iVCount_;

  GLenum      shadingModel_;

  // Backface culling
  bool        cullFaceEnabled_;
  bool        bCullBack_;
  GLenum      cullFaceMode_;

  // Colors
  SColorF     clCurrent;
  SColorF     clClear;

  // Lighting
  bool        lightingEnabled_;
  SLightF     lights_[8];

  // Normals
  GLfloat     normal_[4];

  // Fog
  bool        fogEnabled_;
  GLfloat     fogDensity_;
  GLfloat     fogStart_;
  GLfloat     fogEnd_;
  SColorF     fogColor_;

  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfloat     depthClear_;

  // Viewport
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;
};


IGLESContext * getGLESContext();


#endif // GBA_GLESCONTEXT_H
