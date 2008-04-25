#ifndef SOFTGLF_H
#define SOFTGLF_H


#include "context.h"

#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "kernel/videoManager.h"
#include "glMatrix.h"
#include "textures.h"
#include "edge.h"

#include "asm/arch/memory.h"


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

  // Color
  union
  {
    struct
    {
      GLfloat cr, cg, cb, ca;
    };
    GLfloat c[4];
    SColorF cl;
  };

  // Texture coordinates
  union
  {
    struct
    {
      GLfloat ts, tt;
    };
    struct
    {
      GLfloat tu, tv;
    };
    GLfloat t[2];
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
struct SLightF
{
  SColorF   diffuse;
  SColorF   ambient;
  SColorF   specular;
  SVertex4F position;
  SVector4F direction;
  bool enabled;
};

//-----------------------------------------------------------------------------
class CSoftGLESFloat
 : public CAGLESFxToFloatContext
 , public CAGLESBuffers
 , public CAGLESCull
 , public CAGLESMatrixF
 , public CAGLESTextures
 , public virtual CASoftwareRenderer
{
public:
  CSoftGLESFloat();
  virtual ~CSoftGLESFloat();

  // Surfaces
  virtual void       setSurface(CSurface * surface){CASoftwareRenderer::setSurface(surface);}
  virtual CSurface * getSurface()                  {return CASoftwareRenderer::getSurface();}

  // Flush operations to surface
  virtual void       flush()                       {CASoftwareRenderer::flush();}

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
  virtual void glMaterialf(GLenum face, GLenum pname, GLfloat param);
  virtual void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
  virtual void glShadeModel(GLenum mode);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

protected:
  // Vertex shader
  virtual void vertexShader(SVertexF & v);

  // Rasterizer
  virtual void begin(GLenum mode);
  virtual void rasterize(SVertexF & v);
  virtual void end();

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
  bool        normalizeEnabled_;
  GLfloat     normal_[4];

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

  CEdgeF    * edge1;
  CEdgeF    * edge2;

  // Rasterizer
  GLenum      rasterMode_;
  SVertexF    vertices[3]; // Vertex buffer for triangles
  SVertexF  * polygon[3];  // Pointers to vertex buffer
  bool        bFlipFlop_;  // Triangle strip
  GLint       vertIdx_;    // Current index into vertex buffer
  GLint       viewportXOffset;
  GLint       viewportYOffset;
  GLsizei     viewportPixelCount;
  GLsizei     viewportWidth;
  GLsizei     viewportHeight;

private:
  bool testAndSetDepth(GLfloat z, uint32_t index)               FAST_CODE;
  void hline   (CEdgeF & from, CEdgeF & to, GLint y, SColorF c) FAST_CODE;
  void hlineZ  (CEdgeF & from, CEdgeF & to, GLint y, SColorF c) FAST_CODE;
  void hlineC  (CEdgeF & from, CEdgeF & to, GLint y)            FAST_CODE;
  void hlineZC (CEdgeF & from, CEdgeF & to, GLint y)            FAST_CODE;
  void hlineTa (CEdgeF & from, CEdgeF & to, GLint y)            FAST_CODE;
  void hlineZTa(CEdgeF & from, CEdgeF & to, GLint y)            FAST_CODE;
  void hlineZTp(CEdgeF & from, CEdgeF & to, GLint y)            FAST_CODE;
  void rasterPoly(SVertexF * vtx[3])                            FAST_CODE;
};


#include "softGLF.inl"


#endif // GL_CONTEXT_H
