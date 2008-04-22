#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H


#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "kernel/videoManager.h"
#include "fixedPoint.h"
#include "asm/arch/config.h"


#define clampf(f)    (f < 0.0f ? 0.0f : (f > 1.0f ? 1.0f : f))
#define clampfx(i)   (i < 0 ? 0 : (i > gl_fpfromi(1) ? gl_fpfromi(1) : i))


//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfloat r, g, b, a;
  };
  GLfloat c[4];
} SColorF;

//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfixed r, g, b, a;
  };
  GLfixed c[4];
} SColorFx;

//-----------------------------------------------------------------------------
struct SBufferPointer
{
  GLint size;
  GLenum type;
  GLsizei stride;
  const GLvoid * pointer;
};

//-----------------------------------------------------------------------------
class CAGLESBuffers
 : public virtual I3DRenderer
{
public:
  CAGLESBuffers();
  virtual ~CAGLESBuffers();

  virtual void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glDisableClientState(GLenum array);
  virtual void glEnableClientState(GLenum array);
  virtual void glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  virtual void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);

protected:
  bool bBufColorEnabled_;
  bool bBufNormalEnabled_;
  bool bBufTexCoordEnabled_;
  bool bBufVertexEnabled_;
  SBufferPointer bufColor_;
  SBufferPointer bufNormal_;
  SBufferPointer bufTexCoord_;
  SBufferPointer bufVertex_;
};

//-----------------------------------------------------------------------------
class CAGLESCull
 : public virtual I3DRenderer
{
public:
  CAGLESCull();
  virtual ~CAGLESCull();

  virtual void glCullFace(GLenum mode);
  virtual void glFrontFace(GLenum mode);

protected:
  bool        cullFaceEnabled_;
  GLenum      cullFaceMode_;
  GLenum      frontFace_;
  bool        bCullCW_;
};

#ifndef CONFIG_FPU
//-----------------------------------------------------------------------------
class CAGLESFloatToFxContext
 : public virtual I3DRenderer
{
public:
  virtual ~CAGLESFloatToFxContext(){}

  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void glClearDepthf(GLclampf depth);
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
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
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
  virtual void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
  virtual void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z);
  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
};
#else
//-----------------------------------------------------------------------------
class CAGLESFxToFloatContext
 : public virtual I3DRenderer
{
public:
  virtual ~CAGLESFxToFloatContext(){}

  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
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
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);
};
#endif // CONFIG_FPU


#endif // GL_CONTEXT_H
