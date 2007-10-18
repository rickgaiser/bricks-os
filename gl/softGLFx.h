#ifndef SOFTGLFX_H
#define SOFTGLFX_H


#include "context.h"

#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "kernel/videoManager.h"
#include "matrix.h"
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
  // Original
  union
  {
    struct
    {
      GLfixed vx1, vy1, vz1, vw1;
    };
    GLfixed v1[4];
  };
  // Transformed
  union
  {
    struct
    {
      GLfixed vx2, vy2, vz2, vw2;
    };
    GLfixed v2[4];
  };

  // Normal vector
  // Original
  union
  {
    struct
    {
      GLfixed nx1, ny1, nz1, nw1;
    };
    GLfixed n1[4];
  };
  // Transformed
  union
  {
    struct
    {
      GLfixed nx2, ny2, nz2, nw2;
    };
    GLfixed n2[4];
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
  // Original
  SColorFx  c1;
  // Lighted
  SColorFx  c2;

  // State: indicates if the xxx2 values have been created already
  bool      bProcessed;
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
  virtual void glFlush(void);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glShadeModel(GLenum mode);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);
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
  GLfixed     zFar_;
  GLfixed     zNear_;
  GLfixed     zLoss_;

  GLenum      shadingModel_;

  // Backface culling
  bool        cullFaceEnabled_;
  bool        bCullBack_;
  GLenum      cullFaceMode_;

  // Matrix
  GLenum      matrixMode_;
  CMatrixFx   matrixModelView;
  CMatrixFx   matrixProjection;
  CMatrixFx   matrixPerspective;
  CMatrixFx   matrixRotation;
  CMatrixFx * pCurrentMatrix_;

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
  GLsizei     viewportByteCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;
  GLfixed     fpFieldofviewXScalar;
  GLfixed     fpFieldofviewYScalar;
};


#endif // GL_CONTEXT_H
