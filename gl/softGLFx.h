#ifndef SOFTGLFX_H
#define SOFTGLFX_H


#include "context.h"

#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "kernel/videoManager.h"
#include "matrix.h"
#include "textures.h"
#include "edge.h"
#include "fixedPoint.h"


//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfixed x, y;
  };
  GLfixed v[2];
}SVector2Fx, SVertex2Fx;

//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfixed x, y, z;
  };
  GLfixed v[3];
}SVector3Fx, SVertex3Fx;

//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfixed x, y, z, w;
  };
  GLfixed v[4];
}SVector4Fx, SVertex4Fx;

//-----------------------------------------------------------------------------
struct SVertexFx
{
  // Vertex itself
  union
  {
    struct
    {
      GLfixed vx, vy, vz, vw;
    };
    GLfixed v[4];
  };

  // Normal vector
  union
  {
    struct
    {
      GLfixed nx, ny, nz, nw;
    };
    GLfixed n[4];
  };

  // 2D Point (on screen) x/y
  union
  {
    struct
    {
      GLint sx, sy;
    };
    GLint s[2];
  };

  // Color
  SColorFx  c;

  // State indicates if the values have been processed already
  bool bProcessed;
};

//-----------------------------------------------------------------------------
// Triangle/Polygon
struct SPolygonFx
{
  SVertexFx * v[3];
};

//-----------------------------------------------------------------------------
struct SLightFx
{
  SColorFx diffuse;
  SColorFx ambient;
  SColorFx specular;
  bool enabled;
};

//-----------------------------------------------------------------------------
class CSoftGLESFixed
 : public CAGLESFloatToFxContext
 , public CAGLESBuffers
 , public CAGLESMatrixFx
 , public CAGLESTextures
{
public:
  CSoftGLESFixed();
  virtual ~CSoftGLESFixed();

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
  virtual bool testAndSetDepth(GLfixed z, uint32_t index);
  virtual void hline(CEdgeFx & from, CEdgeFx & to, GLint & y, SColorFx c);
  virtual void hline_s(CEdgeFx & from, CEdgeFx & to, GLint & y);
  virtual void addVertexToTriangle(SVertexFx & v);
  virtual void addVertexToTriangleStrip(SVertexFx & v);
  virtual void addVertexToTriangleFan(SVertexFx & v);
  virtual void plotPoly(SPolygonFx & poly);
  virtual void rasterPoly(SPolygonFx & poly);

protected:
  CSurface  * renderSurface;
  GLint       iVCount_;

  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfixed     depthClear_;
  uint16_t    zClearValue_;
  uint16_t  * zbuffer;
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

  CEdgeFx   * edge1;
  CEdgeFx   * edge2;

  // Viewport
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;
};


#endif // GL_CONTEXT_H
