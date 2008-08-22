#ifndef SOFTGLFX_H
#define SOFTGLFX_H


#include "context.h"

#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "kernel/videoManager.h"
#include "glMatrix.h"
#include "textures.h"
#include "edge.h"
#include "fixedPoint.h"
#include "vector.h"

#include "asm/arch/memory.h"


#define fpRGB(r,g,b) \
  (0x8000 | \
  (((b.value*255) >>  9) & 0x7c00) | \
  (((g.value*255) >> 14) & 0x03e0) | \
  (((r.value*255) >> 19) & 0x001f))


//-----------------------------------------------------------------------------
class CASoftGLESFixed
 : public virtual CAGLESFloatToFxContext
 , public virtual CAGLESBuffers
 , public virtual CAGLESCull
 , public virtual CAGLESMatrixFx
{
public:
  CASoftGLESFixed();
  virtual ~CASoftGLESFixed();

  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glDepthRangex(GLclampx zNear, GLclampx zFar);
  virtual void glDepthFunc(GLenum func);
  virtual void glDisable(GLenum cap);
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count);
  virtual void glEnable(GLenum cap);
  virtual void glFinish(void);
  virtual void glFlush(void);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual GLenum glGetError(void);
  virtual void glHint(GLenum target, GLenum mode);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glMaterialx(GLenum face, GLenum pname, GLfixed param);
  virtual void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
  virtual void glShadeModel(GLenum mode);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  // Vertex shader
  virtual void vertexShaderTransform(SVertexFx & v);
  virtual void vertexShaderLight(SVertexFx & v);

  // Fragment shader
  virtual void fragmentCull(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2);
  virtual void fragmentClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2);

  // Rasterizer
  virtual void begin(GLenum mode);
  virtual void primitiveAssembly(SVertexFx & v);
  virtual void end();

  virtual void rasterTriangleClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2, uint32_t clipBit = 0);
  virtual void rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2) = 0;

  void interpolateVertex(SVertexFx & vNew, SVertexFx & vOld, SVertexFx & vFrom, CFixed t);
  void setError(GLenum error);

protected:
  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfixed     depthClear_;
  uint16_t    zClearValue_;
  GLfixed     zLoss_;
  CFixed      zNear_;
  CFixed      zFar_;
  CFixed      zA_;
  CFixed      zB_;
  uint32_t    zMax_;

  GLenum      shadingModel_;

  // Colors
  SColorFx    clCurrent;
  SColorFx    clClear;

  // Lighting
  bool        lightingEnabled_;
  SLightFx    lights_[8];

  // Normals
  bool        normalizeEnabled_;
  TVector3<CFixed> normal_;

  // Material
  SColorFx    matColorAmbient_;
  SColorFx    matColorDiffuse_;
  SColorFx    matColorSpecular_;
  SColorFx    matColorEmission_;
  CFixed      matShininess_;

  // Fog
  bool        fogEnabled_;
  GLfixed     fogDensity_;
  GLfixed     fogStart_;
  GLfixed     fogEnd_;
  SColorFx    fogColor_;

  // Textures
  bool        texturesEnabled_;

  // Hints
  GLenum      hintFog_;
  GLenum      hintLineSmooth_;
  GLenum      hintPerspectiveCorrection_;
  GLenum      hintPointSmooth_;

  // Error
  GLenum      errorCode_;
  bool        bError_;

  // Vertex transformations
  CFixed      xA_;
  CFixed      xB_;
  CFixed      yA_;
  CFixed      yB_;

  // Primitive assembly
  GLenum      rasterMode_;
  SVertexFx   vertices[3];  // Vertex buffer for primitive assembly
  SVertexFx * triangle_[3]; // Assembled triangle
  bool        bFlipFlop_;   // Triangle strip
  GLint       vertIdx_;     // Current index into vertex buffer

  // Rasterizer
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;

private:
  void _glDrawArrays(GLenum mode, GLint first, GLsizei count)        FAST_CODE;
  void _vertexShaderTransform(SVertexFx & v)                         FAST_CODE;
  void _vertexShaderLight(SVertexFx & v)                             FAST_CODE;
  void _fragmentCull(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2) FAST_CODE;
  void _fragmentClip(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2) FAST_CODE;
  void _primitiveAssembly(SVertexFx & v)                             FAST_CODE;
};

//-----------------------------------------------------------------------------
class CSoftGLESFixed
 : public CASoftGLESFixed
 , public CAGLESTextures
{
public:
  CSoftGLESFixed();
  virtual ~CSoftGLESFixed();

  virtual void glClear(GLbitfield mask);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  virtual void rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2);

protected:
  uint16_t  * zbuffer;
  CEdgeFx   * edge1;
  CEdgeFx   * edge2;

private:
  void _rasterTriangle(SVertexFx & v0, SVertexFx & v1, SVertexFx & v2) FAST_CODE;

  bool testAndSetDepth(GLfixed z, uint32_t index)                      FAST_CODE;
  void hline   (CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c)     FAST_CODE;
  void hlineZ  (CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c)     FAST_CODE;
  void hlineC  (CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
  void hlineZC (CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
  void hlineTa (CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
  void hlineZTa(CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
//  void hlineZTp(CEdgeFx & from, CEdgeFx & to, GLint y)                 FAST_CODE;
};


#include "softGLFx.inl"


#endif // GL_CONTEXT_H
