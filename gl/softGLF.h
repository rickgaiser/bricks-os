#ifndef SOFTGLF_H
#define SOFTGLF_H


#include "context.h"

#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "glMatrix.h"
#include "textures.h"
#include "edge.h"
#include "vhl/vector.h"

#include "asm/arch/memory.h"


//-----------------------------------------------------------------------------
class CASoftGLESFloat
 : public virtual CAGLESFxToFloatContext
 , public virtual CAGLESBase
 , public virtual CAGLESMatrixF
{
public:
  CASoftGLESFloat();
  virtual ~CASoftGLESFloat();

  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void glClearDepthf(GLclampf depth);
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
  virtual void glShadeModel(GLenum mode);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

  virtual void glBegin(GLenum mode);
  virtual void glEnd();
  virtual void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  virtual void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);

protected:
  // Vertex shader
  virtual void vertexShaderTransform(SVertexF & v);
  virtual void vertexShaderLight(SVertexF & v);

  // Fragment shader
  virtual void fragmentCull(SVertexF & v0, SVertexF & v1, SVertexF & v2);
  virtual void fragmentClip(SVertexF & v0, SVertexF & v1, SVertexF & v2);

  // Rasterizer
  virtual void primitiveAssembly(SVertexF & v);

  virtual void rasterTriangleClip(SVertexF & v0, SVertexF & v1, SVertexF & v2, uint32_t clipBit = 0);
  virtual void rasterTriangle(SVertexF & v0, SVertexF & v1, SVertexF & v2) = 0;

  void interpolateVertex(SVertexF & vNew, SVertexF & vOld, SVertexF & vFrom, GLfloat t);

  virtual void zbuffer(bool enable) = 0;

protected:
  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfloat     depthClear_;
  uint32_t    zClearValue_;
  GLclampf    zNear_;
  GLclampf    zFar_;
  GLfloat     zA_;
  GLfloat     zB_;
  uint32_t    zMax_;

  GLenum      shadingModel_;

  // Colors
  SColorF     clCurrent;
  SColorF     clClear;

  // Lighting
  bool        lightingEnabled_;
  SLightF     lights_[8];

  // Normals
  bool        normalizeEnabled_;
  TVector3<GLfloat> normal_;

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
  GLfloat     texCoordCurrent_[4];

  // Vertex transformations
  GLfloat     xA_;
  GLfloat     xB_;
  GLfloat     yA_;
  GLfloat     yB_;

  // Primitive assembly
  GLenum      rasterMode_;
  SVertexF    vertices[3];  // Vertex buffer for primitive assembly
  SVertexF  * triangle_[3]; // Assembled triangle
  bool        bFlipFlop_;   // Triangle strip
  GLint       vertIdx_;     // Current index into vertex buffer

  // Rasterizer
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;

private:
  void _glDrawArrays(GLenum mode, GLint first, GLsizei count)     FAST_CODE;
  void _vertexShaderTransform(SVertexF & v)                       FAST_CODE;
  void _vertexShaderLight(SVertexF & v)                           FAST_CODE;
  void _fragmentCull(SVertexF & v0, SVertexF & v1, SVertexF & v2) FAST_CODE;
  void _fragmentClip(SVertexF & v0, SVertexF & v1, SVertexF & v2) FAST_CODE;
  void _primitiveAssembly(SVertexF & v)                           FAST_CODE;
};

//-----------------------------------------------------------------------------
class CSoftGLESFloat
 : public CASoftGLESFloat
 , public CAGLESTextures
{
public:
  CSoftGLESFloat();
  virtual ~CSoftGLESFloat();

  virtual void glClear(GLbitfield mask);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  virtual void rasterTriangle(SVertexF & v0, SVertexF & v1, SVertexF & v2);
  virtual void zbuffer(bool enable);

private:
  uint32_t  * pZBuffer_;
  CEdgeF    * edge1;
  CEdgeF    * edge2;

private:
  void _rasterTriangle(SVertexF & v0, SVertexF & v1, SVertexF & v2) FAST_CODE;

  bool testAndSetDepth(GLfloat z, uint32_t index)                   FAST_CODE;
  void hline   (CEdgeF & from, CEdgeF & to, GLint y, SColorF c)     FAST_CODE;
  void hlineZ  (CEdgeF & from, CEdgeF & to, GLint y, SColorF c)     FAST_CODE;
  void hlineC  (CEdgeF & from, CEdgeF & to, GLint y)                FAST_CODE;
  void hlineZC (CEdgeF & from, CEdgeF & to, GLint y)                FAST_CODE;
  void hlineTa (CEdgeF & from, CEdgeF & to, GLint y)                FAST_CODE;
  void hlineZTa(CEdgeF & from, CEdgeF & to, GLint y)                FAST_CODE;
  void hlineZTp(CEdgeF & from, CEdgeF & to, GLint y)                FAST_CODE;
};


#include "softGLF.inl"


#endif // GL_CONTEXT_H
