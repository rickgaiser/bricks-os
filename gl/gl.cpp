#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "context.h"


extern void * eglGetCurrentGLESContext();

#define GLES_GET_CONTEXT(RETVAL) \
IGLESContext * context = (IGLESContext *)eglGetCurrentGLESContext(); \
if(context == 0) \
{ \
  return RETVAL; \
}


//-----------------------------------------------------------------------------
// GL API
//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  GLES_GET_CONTEXT();
  context->glClearColor(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClearDepthf(GLclampf depth)
{
  GLES_GET_CONTEXT();
  context->glClearDepthf(depth);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  GLES_GET_CONTEXT();
  context->glColor4f(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glFogf(GLenum pname, GLfloat param)
{
  GLES_GET_CONTEXT();
  context->glFogf(pname, param);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glFogfv(GLenum pname, const GLfloat * params)
{
  GLES_GET_CONTEXT();
  context->glFogfv(pname, params);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  GLES_GET_CONTEXT();
  context->glFrustumf(left, right, bottom, top, zNear, zFar);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param)
{
  GLES_GET_CONTEXT();
  context->glLightf(light, pname, param);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
  GLES_GET_CONTEXT();
  context->glLightfv(light, pname, params);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
  GLES_GET_CONTEXT();
  context->glNormal3f(nx, ny, nz);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  GLES_GET_CONTEXT();
  context->glOrthof(left, right, bottom, top, zNear, zFar);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  GLES_GET_CONTEXT();
  context->glRotatef(angle, x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  GLES_GET_CONTEXT();
  context->glScalef(x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  GLES_GET_CONTEXT();
  context->glTranslatef(x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClear(GLbitfield mask)
{
  GLES_GET_CONTEXT();
  context->glClear(mask);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  GLES_GET_CONTEXT();
  context->glClearColorx(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClearDepthx(GLclampx depth)
{
  GLES_GET_CONTEXT();
  context->glClearDepthx(depth);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  GLES_GET_CONTEXT();
  context->glColor4ub(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  GLES_GET_CONTEXT();
  context->glColor4x(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  GLES_GET_CONTEXT();
  context->glColorPointer(size, type, stride, pointer);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glCullFace(GLenum mode)
{
  GLES_GET_CONTEXT();
  context->glCullFace(mode);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glDepthFunc(GLenum func)
{
  GLES_GET_CONTEXT();
  context->glDepthFunc(func);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glDisable(GLenum cap)
{
  GLES_GET_CONTEXT();
  context->glDisable(cap);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glDisableClientState(GLenum array)
{
  GLES_GET_CONTEXT();
  context->glDisableClientState(array);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  GLES_GET_CONTEXT();
  context->glDrawArrays(mode, first, count);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glEnable(GLenum cap)
{
  GLES_GET_CONTEXT();
  context->glEnable(cap);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glEnableClientState(GLenum array)
{
  GLES_GET_CONTEXT();
  context->glEnableClientState(array);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glFlush(void)
{
  GLES_GET_CONTEXT();
  context->glFlush();
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glFogx(GLenum pname, GLfixed param)
{
  GLES_GET_CONTEXT();
  context->glFogx(pname, param);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glFogxv(GLenum pname, const GLfixed * params)
{
  GLES_GET_CONTEXT();
  context->glFogxv(pname, params);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  GLES_GET_CONTEXT();
  context->glFrustumx(left, right, bottom, top, zNear, zFar);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLightx(GLenum light, GLenum pname, GLfixed param)
{
  GLES_GET_CONTEXT();
  context->glLightx(light, pname, param);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed *params)
{
  GLES_GET_CONTEXT();
  context->glLightxv(light, pname, params);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLoadIdentity()
{
  GLES_GET_CONTEXT();
  context->glLoadIdentity();
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glMatrixMode(GLenum mode)
{
  GLES_GET_CONTEXT();
  context->glMatrixMode(mode);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  GLES_GET_CONTEXT();
  context->glNormal3x(nx, ny, nz);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
  GLES_GET_CONTEXT();
  context->glNormalPointer(type, stride, pointer);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  GLES_GET_CONTEXT();
  context->glOrthox(left, right, bottom, top, zNear, zFar);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  GLES_GET_CONTEXT();
  context->glRotatex(angle, x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  GLES_GET_CONTEXT();
  context->glScalex(x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glShadeModel(GLenum mode)
{
  GLES_GET_CONTEXT();
  context->glShadeModel(mode);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  GLES_GET_CONTEXT();
  context->glTranslatex(x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  GLES_GET_CONTEXT();
  context->glVertexPointer(size, type, stride, pointer);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  GLES_GET_CONTEXT();
  context->glViewport(x, y, width, height);
}
