#ifndef SOFTGLF_H
#define SOFTGLF_H


#include "context.h"

#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "kernel/videoManager.h"
#include "matrix.h"
#include "textures.h"
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
  union
  {
    struct
    {
      GLfloat vx, vy, vz, vw;
    };
    GLfloat v[4];
  };

  // Normal vector
  union
  {
    struct
    {
      GLfloat nx, ny, nz, nw;
    };
    GLfloat n[4];
  };

  // Texture coordinates
  union
  {
    struct
    {
      GLfloat ts, tt;
    };
    GLfloat t[2];
  };

  // 2D Point (on screen) x/y
  union
  {
    struct
    {
      GLint sx, sy, sz;
    };
    GLint s[3];
  };

  // Color
  SColorF  c;

  // State indicates if the values have been processed already
  bool bProcessed;
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
 , public CAGLESCull
 , public CAGLESMatrixF
 , public CAGLESTextures
{
public:
  CSoftGLESFloat();
  virtual ~CSoftGLESFloat();

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
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
  virtual void glShadeModel(GLenum mode);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

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
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;

private:
  bool testAndSetDepth(GLfloat z, uint32_t index);
  void hline(CEdgeF & from, CEdgeF & to, GLint & y, SColorF c);
  void hline_s(CEdgeF & from, CEdgeF & to, GLint & y);
  void hline_ta(CEdgeF & from, CEdgeF & to, GLint & y);
  void hline_tp(CEdgeF & from, CEdgeF & to, GLint & y);
  void plotPoly(SVertexF * vtx[3]);
  void rasterPoly(SVertexF * vtx[3]);
};


#endif // GL_CONTEXT_H
