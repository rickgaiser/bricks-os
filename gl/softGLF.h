#ifndef SOFTGLF_H
#define SOFTGLF_H


#include "context.h"

#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "kernel/videoManager.h"
#include "matrix.h"
#include "edge.h"


//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfloat x, y;
  };
  GLfloat v[2];
}SVector2F, SVertex2F;

//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfloat x, y, z;
  };
  GLfloat v[3];
}SVector3F, SVertex3F;

//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfloat x, y, z, w;
  };
  GLfloat v[4];
}SVector4F, SVertex4F;

//-----------------------------------------------------------------------------
struct SVertexF
{
  // Vertex itself
  // Original
  union
  {
    struct
    {
      GLfloat vx1, vy1, vz1, vw1;
    };
    GLfloat v1[4];
  };
  // Transformed
  union
  {
    struct
    {
      GLfloat vx2, vy2, vz2, vw2;
    };
    GLfloat v2[4];
  };

  // Normal vector
  // Original
  union
  {
    struct
    {
      GLfloat nx1, ny1, nz1, nw1;
    };
    GLfloat n1[4];
  };
  // Transformed
  union
  {
    struct
    {
      GLfloat nx2, ny2, nz2, nw2;
    };
    GLfloat n2[4];
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
  SColorF  c1;
  // Lighted
  SColorF  c2;

  // State: indicates if the xxx2 values have been created already
  bool      bProcessed;
};

//-----------------------------------------------------------------------------
// Triangle/Polygon
struct SPolygonF
{
  SVertexF * v[3];
};

//-----------------------------------------------------------------------------
struct SLightF
{
  SColorF diffuse;
  SColorF ambient;
  SColorF specular;
  bool enabled;
};

//-----------------------------------------------------------------------------
class CSoftGLESFloat
 : public CAGLESFxToFloatContext
 , public CAGLESBuffers
{
public:
  CSoftGLESFloat();
  virtual ~CSoftGLESFloat();

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
  virtual void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
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
  virtual bool testAndSetDepth(GLfloat z, uint32_t index);
  virtual void hline(CEdgeF & from, CEdgeF & to, GLint & y, SColorF c);
  virtual void hline_s(CEdgeF & from, CEdgeF & to, GLint & y);
  virtual void addVertexToTriangle(SVertexF & v);
  virtual void addVertexToTriangleStrip(SVertexF & v);
  virtual void addVertexToTriangleFan(SVertexF & v);
  virtual void plotPoly(SPolygonF & poly);
  virtual void rasterPoly(SPolygonF & poly);

protected:
  CSurface  * renderSurface;
  GLint       iVCount_;

  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfloat     depthClear_;
  uint32_t    zClearValue_;
  uint32_t  * zbuffer;
  GLfloat     zFar_;
  GLfloat     zNear_;
  GLfloat     zA_;
  GLfloat     zB_;

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

  CEdgeF    * edge1;
  CEdgeF    * edge2;

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


#endif // GL_CONTEXT_H
