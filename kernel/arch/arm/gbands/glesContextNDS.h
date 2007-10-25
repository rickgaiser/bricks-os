#ifndef NDS_GLESCONTEXTNDS_H
#define NDS_GLESCONTEXTNDS_H


#include "../../../../gl/softGLFx.h"
#include "../../../../gl/fixedPoint.h"
#include "matrixNDS.h"


//-----------------------------------------------------------------------------
class CNDSGLESContext
 : public CAGLESFloatToFxContext
 , public CAGLESBuffers
 , public CAGLESMatrixNDSFx
{
public:
  CNDSGLESContext();
  virtual ~CNDSGLESContext();

  virtual void setSurface(CSurface * surface);

  virtual void glClear(GLbitfield mask);
  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glCullFace(GLenum mode);
  virtual void glDepthFunc(GLenum func);
  virtual void glDisable(GLenum cap);
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count);
  virtual void glEnable(GLenum cap);
  virtual void glFinish(void);
  virtual void glFlush(void);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
  virtual void glShadeModel(GLenum mode);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  virtual void addVertexToTriangleFan(SVertexFx & v);
  virtual void plotPoly(SPolygonFx & poly);

protected:
  CSurface  * renderSurface;
  GLint       iVCount_;

  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfixed     depthClear_;
  uint16_t    zClearValue_;
  GLfixed     zLoss_;

  GLenum      shadingModel_;

  // Backface culling
  bool        cullFaceEnabled_;
  bool        bCullBack_;
  GLenum      cullFaceMode_;

  // Colors
  SColorFx    clCurrent;
  SColorFx    clClear;

  // Lighting
  bool        lightingEnabled_;
  SLightFx    lights_[8];

  // Normals
  GLfixed     normal_[4];

  // Fog
  bool        fogEnabled_;
  GLfixed     fogDensity_;
  GLfixed     fogStart_;
  GLfixed     fogEnd_;
  SColorFx    fogColor_;

  // Viewport
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;
};


IGLESContext * getGLESContext();


#endif // NDS_GLESCONTEXTNDS_H
