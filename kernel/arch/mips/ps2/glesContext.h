#ifndef PS2_GLESCONTEXT_H
#define PS2_GLESCONTEXT_H


#include "../../../../gl/softGLF.h"


//-----------------------------------------------------------------------------
class CPS2GLESContext
 : public CAGLESFxToFloatContext
 , public CAGLESBuffers
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
  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
  virtual void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z);
  virtual void glShadeModel(GLenum mode);
  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
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

  GLfloat     zFar_;
  GLfloat     zNear_;
  GLfloat     zA_;
  GLfloat     zB_;

  CSurface  * renderSurface;
  GLint       iVCount_;

  GLenum      shadingModel_;

  // Backface culling
  bool        cullFaceEnabled_;
  bool        bCullBack_;
  GLenum      cullFaceMode_;

  // Matrix
  GLenum      matrixMode_;
  CMatrixF    matrixModelView;
  CMatrixF    matrixProjection;
  CMatrixF    matrixPerspective;
  CMatrixF    matrixRotation;
  CMatrixF  * pCurrentMatrix_;

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
  GLsizei     viewportByteCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;
  GLfloat     fpFieldofviewXScalar;
  GLfloat     fpFieldofviewYScalar;
};


IGLESContext * getGLESContext();


#endif // GBA_GLESCONTEXT_H
