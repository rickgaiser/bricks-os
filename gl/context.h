#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H


#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "vhl/fixedPoint.h"
#include "vhl/vector.h"
#include "vhl/color.h"
#include "asm/arch/config.h"


#define clampf(f)    ((f) < 0.0f ? 0.0f : ((f) > 1.0f ? 1.0f : (f)))
#define clampfx(i)   ((i) < 0 ? 0 : ((i) > gl_fpfromi(1) ? gl_fpfromi(1) : (i)))

// Clipping flags
#define CLIP_X_MIN (1<<0)
#define CLIP_X_MAX (1<<1)
#define CLIP_Y_MIN (1<<2)
#define CLIP_Y_MAX (1<<3)
#define CLIP_Z_MIN (1<<4)
#define CLIP_Z_MAX (1<<5)


//-----------------------------------------------------------------------------
typedef TColor<GLfloat> SColorF;
typedef TColor<CFixed>  SColorFx;

//-----------------------------------------------------------------------------
template <class T>
struct TVertex
{
  // Vertex itself
  TVector4<T> vo; // Object Coordinates
  TVector4<T> ve; // Eye Coordinates
  TVector4<T> vc; // Clip Coordinates
  TVector4<T> vd; // Normalized Device Coordinates

  // Normal vector
  TVector3<T> n;  // Vertex Normal
  TVector3<T> n2; // Vertex Normal, rotated

  // Color
  TColor<T> cl;  // Vertex Color

  // Texture coordinates
  T t[2];

  // Window coordinates
  GLint  sx, sy;

  // Depth (on screen)
  uint32_t sz;

  // Clipping flags
  uint32_t clip;

  // Processed by post vertex shader
  bool processed;
};
typedef TVertex<GLfloat> SVertexF;
typedef TVertex<CFixed>  SVertexFx;

//-----------------------------------------------------------------------------
template <class T>
struct TLight
{
  TColor<T> diffuse;
  TColor<T> ambient;
  TColor<T> specular;
  TVector4<T> position;
  TVector3<T> direction;
  bool enabled;
};
typedef TLight<GLfloat> SLightF;
typedef TLight<CFixed>  SLightFx;

//-----------------------------------------------------------------------------
struct SBufferPointer
{
  GLint size;
  GLenum type;
  GLsizei stride;
  const GLvoid * pointer;
};

//-----------------------------------------------------------------------------
class CAGLESBase
 : public virtual I3DRenderer
{
public:
  CAGLESBase();
  virtual ~CAGLESBase();

  virtual void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glDisableClientState(GLenum array);
  virtual void glEnableClientState(GLenum array);
  virtual void glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  virtual void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);

  virtual void glCullFace(GLenum mode);
  virtual void glFrontFace(GLenum mode);

  virtual void glHint(GLenum target, GLenum mode);

  virtual GLenum glGetError(void);

protected:
  void setError(GLenum error);

protected:
  // Buffers
  bool bBufColorEnabled_;
  bool bBufNormalEnabled_;
  bool bBufTexCoordEnabled_;
  bool bBufVertexEnabled_;
  SBufferPointer bufColor_;
  SBufferPointer bufNormal_;
  SBufferPointer bufTexCoord_;
  SBufferPointer bufVertex_;

  // Culling
  bool        cullFaceEnabled_;
  GLenum      cullFaceMode_;
  GLenum      frontFace_;
  bool        bCullCW_;

  // Hints
  GLenum      hintFog_;
  GLenum      hintLineSmooth_;
  GLenum      hintPerspectiveCorrection_;
  GLenum      hintPointSmooth_;

  // Error
  GLenum      errorCode_;
  bool        bError_;
};

#ifndef CONFIG_FPU
//-----------------------------------------------------------------------------
class CAGLESFloatToFxContext
 : public virtual I3DRenderer
{
public:
  CAGLESFloatToFxContext(){}
  virtual ~CAGLESFloatToFxContext(){}

  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void glClearDepthf(GLclampf depth);
  virtual void glDepthRangef(GLclampf zNear, GLclampf zFar);
  virtual void glFogf(GLenum pname, GLfloat param);
  virtual void glFogfv(GLenum pname, const GLfloat *params);
  virtual void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  virtual void glLightf(GLenum light, GLenum pname, GLfloat param);
  virtual void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
  virtual void glLoadMatrixf(const GLfloat *m);
  virtual void glMaterialf(GLenum face, GLenum pname, GLfloat param);
  virtual void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
  virtual void glMultMatrixf(const GLfloat *m);
  virtual void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  virtual void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z);
  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z);

  virtual void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  virtual void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
};
#else
//-----------------------------------------------------------------------------
class CAGLESFxToFloatContext
 : public virtual I3DRenderer
{
public:
  CAGLESFxToFloatContext(){}
  virtual ~CAGLESFxToFloatContext(){}

  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glDepthRangex(GLclampx zNear, GLclampx zFar);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glLoadMatrixx(const GLfixed *m);
  virtual void glMaterialx(GLenum face, GLenum pname, GLfixed param);
  virtual void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params);
  virtual void glMultMatrixx(const GLfixed *m);
  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);

  virtual void glVertex4x(GLfixed x, GLfixed y, GLfixed z, GLfixed w);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glTexCoord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
};
#endif // CONFIG_FPU


#endif // GL_CONTEXT_H
