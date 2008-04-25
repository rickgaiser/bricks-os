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

#include "asm/arch/memory.h"


#define fpRGB(r,g,b) \
  (0x8000 | \
  (((b*255) >>  9) & 0x7c00) | \
  (((g*255) >> 14) & 0x03e0) | \
  (((r*255) >> 19) & 0x001f))


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

  // Color
  union
  {
    struct
    {
      GLfloat cr, cg, cb, ca;
    };
    GLfloat c[4];
    SColorFx cl;
  };

  // Texture coordinates
  union
  {
    struct
    {
      GLfixed ts, tt;
    };
    struct
    {
      GLfixed tu, tv;
    };
    GLfixed t[2];
  };

  // 2D Point (on screen) x/y
  union
  {
    struct
    {
      GLint sx, sy;
      uint32_t sz;
    };
    GLint s[3];
  };
};

//-----------------------------------------------------------------------------
struct SLightFx
{
  SColorFx   diffuse;
  SColorFx   ambient;
  SColorFx   specular;
  SVertex4Fx position;
  SVector4Fx direction;
  bool enabled;
};

//-----------------------------------------------------------------------------
class CSoftGLESFixed
 : public CAGLESFloatToFxContext
 , public CAGLESBuffers
 , public CAGLESCull
 , public CAGLESMatrixFx
 , public CAGLESTextures
 , public virtual CASoftwareRenderer
{
public:
  CSoftGLESFixed();
  virtual ~CSoftGLESFixed();

  // Surfaces
  virtual void       setSurface(CSurface * surface){CASoftwareRenderer::setSurface(surface);}
  virtual CSurface * getSurface()                  {return CASoftwareRenderer::getSurface();}

  // Flush operations to surface
  virtual void       flush()                       {CASoftwareRenderer::flush();}

  virtual void glClear(GLbitfield mask);
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
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glMaterialx(GLenum face, GLenum pname, GLfixed param);
  virtual void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
  virtual void glShadeModel(GLenum mode);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  // Vertex shader
  virtual void vertexShader(SVertexFx & v);

  // Rasterizer
  virtual void begin(GLenum mode);
  virtual void rasterize(SVertexFx & v);
  virtual void end();

protected:
  // Depth testing
  bool        depthTestEnabled_;
  GLenum      depthFunction_;
  GLfixed     depthClear_;
  uint16_t    zClearValue_;
  uint16_t  * zbuffer;
  GLfixed     zLoss_;
  GLclampx    zNear_;
  GLclampx    zFar_;

  GLenum      shadingModel_;

  // Colors
  SColorFx    clCurrent;
  SColorFx    clClear;

  // Lighting
  bool        lightingEnabled_;
  SLightFx    lights_[8];

  // Normals
  bool        normalizeEnabled_;
  GLfixed     normal_[4];

  // Material
  SColorFx    matColorAmbient_;
  SColorFx    matColorDiffuse_;
  SColorFx    matColorSpecular_;
  SColorFx    matColorEmission_;
  GLfixed     matShininess_;

  // Fog
  bool        fogEnabled_;
  GLfixed     fogDensity_;
  GLfixed     fogStart_;
  GLfixed     fogEnd_;
  SColorFx    fogColor_;

  CEdgeFx   * edge1;
  CEdgeFx   * edge2;

  // Rasterizer
  GLenum      rasterMode_;
  SVertexFx   vertices[3]; // Vertex buffer for triangles
  SVertexFx * polygon[3];  // Pointers to vertex buffer
  bool        bFlipFlop_;  // Triangle strip
  GLint       vertIdx_;    // Current index into vertex buffer
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;

private:
  bool testAndSetDepth(GLfixed z, uint32_t index)                  FAST_CODE;
  void hline   (CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c) FAST_CODE;
  void hlineZ  (CEdgeFx & from, CEdgeFx & to, GLint y, SColorFx c) FAST_CODE;
  void hlineC  (CEdgeFx & from, CEdgeFx & to, GLint y)             FAST_CODE;
  void hlineZC (CEdgeFx & from, CEdgeFx & to, GLint y)             FAST_CODE;
  void hlineTa (CEdgeFx & from, CEdgeFx & to, GLint y)             FAST_CODE;
  void hlineZTa(CEdgeFx & from, CEdgeFx & to, GLint y)             FAST_CODE;
  void hlineZTp(CEdgeFx & from, CEdgeFx & to, GLint y)             FAST_CODE;
  void rasterPoly(SVertexFx * vtx[3])                              FAST_CODE;
};


#include "softGLFx.inl"


#endif // GL_CONTEXT_H
