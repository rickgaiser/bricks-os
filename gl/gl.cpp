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
// GL API Macros
//-----------------------------------------------------------------------------
#define GL_APIFUNCTION_0(name) \
GL_API void \
GL_APIENTRY name(void) \
{ \
  GLES_GET_CONTEXT(); \
  context->name(); \
}

#define GL_APIFUNCTION_1(name,type1,arg1) \
GL_API void \
GL_APIENTRY name(type1 arg1) \
{ \
  GLES_GET_CONTEXT(); \
  context->name(arg1); \
}

#define GL_APIFUNCTION_2(name,type1,arg1,type2,arg2) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2) \
{ \
  GLES_GET_CONTEXT(); \
  context->name(arg1, arg2); \
}

#define GL_APIFUNCTION_3(name,type1,arg1,type2,arg2,type3,arg3) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3) \
{ \
  GLES_GET_CONTEXT(); \
  context->name(arg1, arg2, arg3); \
}

#define GL_APIFUNCTION_4(name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
  GLES_GET_CONTEXT(); \
  context->name(arg1, arg2, arg3, arg4); \
}

#define GL_APIFUNCTION_5(name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) \
{ \
  GLES_GET_CONTEXT(); \
  context->name(arg1, arg2, arg3, arg4, arg5); \
}

#define GL_APIFUNCTION_6(name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6) \
{ \
  GLES_GET_CONTEXT(); \
  context->name(arg1, arg2, arg3, arg4, arg5, arg6); \
}

//-----------------------------------------------------------------------------
// GL API (44 of 145 implemented, 30%)
//-----------------------------------------------------------------------------
//GL_API void GL_APIENTRY glAlphaFunc (GLenum func, GLclampf ref);
GL_APIFUNCTION_4(glClearColor,         GLclampf, red, GLclampf, green, GLclampf, blue, GLclampf, alpha);
GL_APIFUNCTION_1(glClearDepthf,        GLclampf, depth);
//GL_API void GL_APIENTRY glClipPlanef (GLenum plane, const GLfloat *equation);
GL_APIFUNCTION_4(glColor4f,            GLfloat, red, GLfloat, green, GLfloat, blue, GLfloat, alpha);
//GL_API void GL_APIENTRY glDepthRangef (GLclampf zNear, GLclampf zFar);
GL_APIFUNCTION_2(glFogf,               GLenum, pname, GLfloat, param);
GL_APIFUNCTION_2(glFogfv,              GLenum, pname, const GLfloat *, params);
GL_APIFUNCTION_6(glFrustumf,           GLfloat, left, GLfloat, right, GLfloat, bottom, GLfloat, top, GLfloat, zNear, GLfloat, zFar);
//GL_API void GL_APIENTRY glGetClipPlanef (GLenum pname, GLfloat eqn[4]);
//GL_API void GL_APIENTRY glGetFloatv (GLenum pname, GLfloat *params);
//GL_API void GL_APIENTRY glGetLightfv (GLenum light, GLenum pname, GLfloat *params);
//GL_API void GL_APIENTRY glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params);
//GL_API void GL_APIENTRY glGetTexEnvfv (GLenum env, GLenum pname, GLfloat *params);
//GL_API void GL_APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
//GL_API void GL_APIENTRY glLightModelf (GLenum pname, GLfloat param);
//GL_API void GL_APIENTRY glLightModelfv (GLenum pname, const GLfloat *params);
GL_APIFUNCTION_3(glLightf,             GLenum, light, GLenum, pname, GLfloat, param);
GL_APIFUNCTION_3(glLightfv,            GLenum, light, GLenum, pname, const GLfloat *, params);
//GL_API void GL_APIENTRY glLineWidth (GLfloat width);
//GL_API void GL_APIENTRY glLoadMatrixf (const GLfloat *m);
//GL_API void GL_APIENTRY glMaterialf (GLenum face, GLenum pname, GLfloat param);
//GL_API void GL_APIENTRY glMaterialfv (GLenum face, GLenum pname, const GLfloat *params);
//GL_API void GL_APIENTRY glMultMatrixf (const GLfloat *m);
//GL_API void GL_APIENTRY glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GL_APIFUNCTION_3(glNormal3f,           GLfloat, nx, GLfloat, ny, GLfloat, nz);
GL_APIFUNCTION_6(glOrthof,             GLfloat, left, GLfloat, right, GLfloat, bottom, GLfloat, top, GLfloat, zNear, GLfloat, zFar);
//GL_API void GL_APIENTRY glPointParameterf (GLenum pname, GLfloat param);
//GL_API void GL_APIENTRY glPointParameterfv (GLenum pname, const GLfloat *params);
//GL_API void GL_APIENTRY glPointSize (GLfloat size);
//GL_API void GL_APIENTRY glPolygonOffset (GLfloat factor, GLfloat units);
GL_APIFUNCTION_4(glRotatef,            GLfloat, angle, GLfloat, x, GLfloat, y, GLfloat, z);
GL_APIFUNCTION_3(glScalef,             GLfloat, x, GLfloat, y, GLfloat, z);
//GL_API void GL_APIENTRY glTexEnvf (GLenum target, GLenum pname, GLfloat param);
//GL_API void GL_APIENTRY glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params);
//GL_API void GL_APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
//GL_API void GL_APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
GL_APIFUNCTION_3(glTranslatef,         GLfloat, x, GLfloat, y, GLfloat, z);

//GL_API void GL_APIENTRY glActiveTexture (GLenum texture);
//GL_API void GL_APIENTRY glAlphaFuncx (GLenum func, GLclampx ref);
//GL_API void GL_APIENTRY glBindBuffer (GLenum target, GLuint buffer);
//GL_API void GL_APIENTRY glBindTexture (GLenum target, GLuint texture);
//GL_API void GL_APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
//GL_API void GL_APIENTRY glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
//GL_API void GL_APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
GL_APIFUNCTION_1(glClear,              GLbitfield, mask);
GL_APIFUNCTION_4(glClearColorx,        GLclampx, red, GLclampx, green, GLclampx, blue, GLclampx, alpha);
GL_APIFUNCTION_1(glClearDepthx,        GLclampx, depth);
//GL_API void GL_APIENTRY glClearStencil (GLint s);
//GL_API void GL_APIENTRY glClientActiveTexture (GLenum texture);
//GL_API void GL_APIENTRY glClipPlanex (GLenum plane, const GLfixed *equation);
GL_APIFUNCTION_4(glColor4ub,           GLubyte, red, GLubyte, green, GLubyte, blue, GLubyte, alpha);
GL_APIFUNCTION_4(glColor4x,            GLfixed, red, GLfixed, green, GLfixed, blue, GLfixed, alpha);
//GL_API void GL_APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
GL_APIFUNCTION_4(glColorPointer,       GLint, size, GLenum, type, GLsizei, stride, const GLvoid *, pointer);
//GL_API void GL_APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
//GL_API void GL_APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
//GL_API void GL_APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
//GL_API void GL_APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GL_APIFUNCTION_1(glCullFace,           GLenum, mode);
//GL_API void GL_APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers);
//GL_API void GL_APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
GL_APIFUNCTION_1(glDepthFunc,          GLenum, func);
//GL_API void GL_APIENTRY glDepthMask (GLboolean flag);
//GL_API void GL_APIENTRY glDepthRangex (GLclampx zNear, GLclampx zFar);
GL_APIFUNCTION_1(glDisable,            GLenum, cap);
GL_APIFUNCTION_1(glDisableClientState, GLenum, array);
GL_APIFUNCTION_3(glDrawArrays,         GLenum, mode, GLint, first, GLsizei, count);
//GL_API void GL_APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
GL_APIFUNCTION_1(glEnable,             GLenum, cap);
GL_APIFUNCTION_1(glEnableClientState,  GLenum, array);
GL_APIFUNCTION_0(glFinish);
GL_APIFUNCTION_0(glFlush);
GL_APIFUNCTION_2(glFogx,               GLenum, pname, GLfixed, param);
GL_APIFUNCTION_2(glFogxv,              GLenum, pname, const GLfixed *, params);
//GL_API void GL_APIENTRY glFrontFace (GLenum mode);
GL_APIFUNCTION_6(glFrustumx,           GLfixed, left, GLfixed, right, GLfixed, bottom, GLfixed, top, GLfixed, zNear, GLfixed, zFar);
//GL_API void GL_APIENTRY glGetBooleanv (GLenum pname, GLboolean *params);
//GL_API void GL_APIENTRY glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params);
//GL_API void GL_APIENTRY glGetClipPlanex (GLenum pname, GLfixed eqn[4]);
//GL_API void GL_APIENTRY glGenBuffers (GLsizei n, GLuint *buffers);
//GL_API void GL_APIENTRY glGenTextures (GLsizei n, GLuint *textures);
//GL_API GLenum GL_APIENTRY glGetError (void);
//GL_API void GL_APIENTRY glGetFixedv (GLenum pname, GLfixed *params);
//GL_API void GL_APIENTRY glGetIntegerv (GLenum pname, GLint *params);
//GL_API void GL_APIENTRY glGetLightxv (GLenum light, GLenum pname, GLfixed *params);
//GL_API void GL_APIENTRY glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params);
//GL_API void GL_APIENTRY glGetPointerv (GLenum pname, void **params);
//GL_API const GLubyte * GL_APIENTRY glGetString (GLenum name);
//GL_API void GL_APIENTRY glGetTexEnviv (GLenum env, GLenum pname, GLint *params);
//GL_API void GL_APIENTRY glGetTexEnvxv (GLenum env, GLenum pname, GLfixed *params);
//GL_API void GL_APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
//GL_API void GL_APIENTRY glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params);
//GL_API void GL_APIENTRY glHint (GLenum target, GLenum mode);
//GL_API GLboolean GL_APIENTRY glIsBuffer (GLuint buffer);
//GL_API GLboolean GL_APIENTRY glIsEnabled (GLenum cap);
//GL_API GLboolean GL_APIENTRY glIsTexture (GLuint texture);
//GL_API void GL_APIENTRY glLightModelx (GLenum pname, GLfixed param);
//GL_API void GL_APIENTRY glLightModelxv (GLenum pname, const GLfixed *params);
GL_APIFUNCTION_3(glLightx,             GLenum, light, GLenum, pname, GLfixed, param);
GL_APIFUNCTION_3(glLightxv,            GLenum, light, GLenum, pname, const GLfixed *, params);
//GL_API void GL_APIENTRY glLineWidthx (GLfixed width);
GL_APIFUNCTION_0(glLoadIdentity);
//GL_API void GL_APIENTRY glLoadMatrixx (const GLfixed *m);
//GL_API void GL_APIENTRY glLogicOp (GLenum opcode);
//GL_API void GL_APIENTRY glMaterialx (GLenum face, GLenum pname, GLfixed param);
//GL_API void GL_APIENTRY glMaterialxv (GLenum face, GLenum pname, const GLfixed *params);
GL_APIFUNCTION_1(glMatrixMode,         GLenum, mode);
//GL_API void GL_APIENTRY glMultMatrixx (const GLfixed *m);
//GL_API void GL_APIENTRY glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
GL_APIFUNCTION_3(glNormal3x,           GLfixed, nx, GLfixed, ny, GLfixed, nz);
GL_APIFUNCTION_3(glNormalPointer,      GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_6(glOrthox,             GLfixed, left, GLfixed, right, GLfixed, bottom, GLfixed, top, GLfixed, zNear, GLfixed, zFar);
//GL_API void GL_APIENTRY glPixelStorei (GLenum pname, GLint param);
//GL_API void GL_APIENTRY glPointParameterx (GLenum pname, GLfixed param);
//GL_API void GL_APIENTRY glPointParameterxv (GLenum pname, const GLfixed *params);
//GL_API void GL_APIENTRY glPointSizex (GLfixed size);
//GL_API void GL_APIENTRY glPolygonOffsetx (GLfixed factor, GLfixed units);
//GL_API void GL_APIENTRY glPopMatrix (void);
//GL_API void GL_APIENTRY glPushMatrix (void);
//GL_API void GL_APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
GL_APIFUNCTION_4(glRotatex,            GLfixed, angle, GLfixed, x, GLfixed, y, GLfixed, z);
//GL_API void GL_APIENTRY glSampleCoverage (GLclampf value, GLboolean invert);
//GL_API void GL_APIENTRY glSampleCoveragex (GLclampx value, GLboolean invert);
GL_APIFUNCTION_3(glScalex,             GLfixed, x, GLfixed, y, GLfixed, z);
//GL_API void GL_APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
GL_APIFUNCTION_1(glShadeModel,         GLenum, mode);
//GL_API void GL_APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
//GL_API void GL_APIENTRY glStencilMask (GLuint mask);
//GL_API void GL_APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
//GL_API void GL_APIENTRY glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
//GL_API void GL_APIENTRY glTexEnvi (GLenum target, GLenum pname, GLint param);
//GL_API void GL_APIENTRY glTexEnvx (GLenum target, GLenum pname, GLfixed param);
//GL_API void GL_APIENTRY glTexEnviv (GLenum target, GLenum pname, const GLint *params);
//GL_API void GL_APIENTRY glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params);
//GL_API void GL_APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
//GL_API void GL_APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
//GL_API void GL_APIENTRY glTexParameterx (GLenum target, GLenum pname, GLfixed param);
//GL_API void GL_APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
//GL_API void GL_APIENTRY glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params);
//GL_API void GL_APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
GL_APIFUNCTION_3(glTranslatex,         GLfixed, x, GLfixed, y, GLfixed, z);
GL_APIFUNCTION_4(glVertexPointer,      GLint, size, GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_4(glViewport,           GLint, x, GLint, y, GLsizei, width, GLsizei, height);
