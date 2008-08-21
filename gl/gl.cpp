#include "GLES/gl.h"
#include "GLES/gl_extra.h"
#include "context.h"
#include "asm/arch/config.h"


I3DRenderer * pCurrentContext = 0;
#define GLES_GET_CONTEXT(RETVAL) \
if(pCurrentContext == 0) \
  return RETVAL;


//-----------------------------------------------------------------------------
void
glMakeCurrent(I3DRenderer * ctx)
{
  pCurrentContext = ctx;
}

//-----------------------------------------------------------------------------
// GL API Macros
//-----------------------------------------------------------------------------
#define GL_APIFUNCTION_0(name) \
GL_API void \
GL_APIENTRY name(void) \
{ \
  GLES_GET_CONTEXT(); \
  pCurrentContext->name(); \
}

#define GL_APIFUNCTION_1(name,type1,arg1) \
GL_API void \
GL_APIENTRY name(type1 arg1) \
{ \
  GLES_GET_CONTEXT(); \
  pCurrentContext->name(arg1); \
}

#define GL_APIFUNCTION_2(name,type1,arg1,type2,arg2) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2) \
{ \
  GLES_GET_CONTEXT(); \
  pCurrentContext->name(arg1, arg2); \
}

#define GL_APIFUNCTION_3(name,type1,arg1,type2,arg2,type3,arg3) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3) \
{ \
  GLES_GET_CONTEXT(); \
  pCurrentContext->name(arg1, arg2, arg3); \
}

#define GL_APIFUNCTION_4(name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
  GLES_GET_CONTEXT(); \
  pCurrentContext->name(arg1, arg2, arg3, arg4); \
}

#define GL_APIFUNCTION_5(name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5) \
{ \
  GLES_GET_CONTEXT(); \
  pCurrentContext->name(arg1, arg2, arg3, arg4, arg5); \
}

#define GL_APIFUNCTION_6(name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6) \
{ \
  GLES_GET_CONTEXT(); \
  pCurrentContext->name(arg1, arg2, arg3, arg4, arg5, arg6); \
}

#define GL_APIFUNCTION_9(name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6,type7,arg7,type8,arg8,type9,arg9) \
GL_API void \
GL_APIENTRY name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6, type7 arg7, type8 arg8, type9 arg9) \
{ \
  GLES_GET_CONTEXT(); \
  pCurrentContext->name(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
}

//-----------------------------------------------------------------------------
// GL API
//-----------------------------------------------------------------------------
//#ifdef OPENGL_ES
//GL_API GLenum GL_APIENTRY glGetError (void);
GL_APIFUNCTION_4(glColor4ub,           GLubyte, red, GLubyte, green, GLubyte, blue, GLubyte, alpha);
GL_APIFUNCTION_4(glColorPointer,       GLint, size, GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_3(glNormalPointer,      GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_4(glTexCoordPointer,    GLint, size, GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_4(glVertexPointer,      GLint, size, GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_3(glDrawArrays,         GLenum, mode, GLint, first, GLsizei, count);
//GL_APIFUNCTION_4(glDrawElements,       GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
//GL_APIFUNCTION_1(glClientActiveTexture, GLenum texture);
GL_APIFUNCTION_1(glDisableClientState, GLenum, array);
GL_APIFUNCTION_1(glEnableClientState,  GLenum, array);
//GL_APIFUNCTION_2(glBindBuffer,         GLenum target, GLuint buffer);
//GL_APIFUNCTION_4(glBufferData,         GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
//GL_APIFUNCTION_4(glBufferSubData,      GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
//GL_APIFUNCTION_2(glDeleteBuffers,      GLsizei n, const GLuint *buffers);
//GL_APIFUNCTION_2(glGenBuffers,         GLsizei n, GLuint *buffers);
GL_APIFUNCTION_4(glViewport,           GLint, x, GLint, y, GLsizei, width, GLsizei, height);
GL_APIFUNCTION_1(glMatrixMode,         GLenum, mode);
GL_APIFUNCTION_0(glLoadIdentity);
//GL_APIFUNCTION_1(glActiveTexture,      GLenum texture);
GL_APIFUNCTION_0(glPushMatrix);
GL_APIFUNCTION_0(glPopMatrix);
GL_APIFUNCTION_1(glDisable,            GLenum, cap);
GL_APIFUNCTION_1(glEnable,             GLenum, cap);
GL_APIFUNCTION_1(glFrontFace,          GLenum, mode);
GL_APIFUNCTION_1(glShadeModel,         GLenum, mode);
GL_APIFUNCTION_1(glCullFace,           GLenum, mode);
//GL_APIFUNCTION_2(glPixelStorei,        GLenum pname, GLint param);
GL_APIFUNCTION_9(glTexImage2D,         GLenum, target, GLint, level, GLint, internalformat, GLsizei, width, GLsizei, height, GLint, border, GLenum, format, GLenum, type, const GLvoid *, pixels);
//GL_APIFUNCTION_9(glTexSubImage2D,      GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
//GL_APIFUNCTION_8(glCopyTexImage2D,     GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
//GL_APIFUNCTION_8(glCopyTexSubImage2D,  GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
//GL_APIFUNCTION_8(glCompressedTexImage2D, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
//GL_APIFUNCTION_9(glCompressedTexSubImage2D, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
//GL_APIFUNCTION_3(glTexParameteri,      GLenum target, GLenum pname, GLint param);
//GL_APIFUNCTION_3(glTexParameteriv,     GLenum target, GLenum pname, const GLint *params);
GL_APIFUNCTION_2(glBindTexture,        GLenum, target, GLuint, texture);
GL_APIFUNCTION_2(glDeleteTextures,     GLsizei, n, const GLuint *, textures);
GL_APIFUNCTION_2(glGenTextures,        GLsizei, n, GLuint *, textures);
//GL_API GLboolean GL_APIENTRY glIsTexture (GLuint texture);
//GL_APIFUNCTION_3(glTexEnvi,            GLenum target, GLenum pname, GLint param);
//GL_APIFUNCTION_3(glTexEnviv,           GLenum target, GLenum pname, const GLint *params);
//GL_APIFUNCTION_3(glGetTexEnviv,        GLenum env, GLenum pname, GLint *params);
//GL_APIFUNCTION_3(glGetTexParameteriv,  GLenum target, GLenum pname, GLint *params);
//GL_APIFUNCTION_4(glScissor,            GLint x, GLint y, GLsizei width, GLsizei height);
//GL_APIFUNCTION_3(glStencilFunc,        GLenum func, GLint ref, GLuint mask);
//GL_APIFUNCTION_3(glStencilOp,          GLenum fail, GLenum zfail, GLenum zpass);
GL_APIFUNCTION_1(glDepthFunc,          GLenum, func);
//GL_APIFUNCTION_2(glBlendFunc,          GLenum sfactor, GLenum dfactor);
//GL_APIFUNCTION_1(glLogicOp,            GLenum opcode);
//GL_APIFUNCTION_4(glColorMask,          GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
//GL_APIFUNCTION_1(glDepthMask,          GLboolean flag);
//GL_APIFUNCTION_1(glStencilMask,        GLuint mask);
GL_APIFUNCTION_1(glClear,              GLbitfield, mask);
//GL_APIFUNCTION_1(glClearStencil,       GLint s);
//GL_APIFUNCTION_7(glReadPixels,         GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
GL_APIFUNCTION_0(glFlush);
GL_APIFUNCTION_0(glFinish);
//GL_APIFUNCTION_2(glHint,               GLenum target, GLenum mode);
//GL_APIFUNCTION_2(glGetBooleanv,        GLenum pname, GLboolean *params);
//GL_APIFUNCTION_2(glGetIntegerv,        GLenum pname, GLint *params);
//GL_API GLboolean GL_APIENTRY glIsEnabled (GLenum cap);
//GL_APIFUNCTION_3(glGetBufferParameteriv, GLenum target, GLenum pname, GLint *params);
//GL_APIFUNCTION_2(glGetPointerv,        GLenum pname, void **params);
//GL_API const GLubyte * GL_APIENTRY glGetString (GLenum name);
//GL_API GLboolean GL_APIENTRY glIsBuffer (GLuint buffer);

//#ifdef OPENGL_ES_COMMON_LITE

GL_APIFUNCTION_3(glNormal3x,           GLfixed, nx, GLfixed, ny, GLfixed, nz);
//GL_APIFUNCTION_5(glMultiTexCoord4x,    GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
GL_APIFUNCTION_4(glColor4x,            GLfixed, red, GLfixed, green, GLfixed, blue, GLfixed, alpha);
GL_APIFUNCTION_2(glDepthRangex,        GLclampx, zNear, GLclampx, zFar);
GL_APIFUNCTION_1(glLoadMatrixx,        const GLfixed *, m);
GL_APIFUNCTION_1(glMultMatrixx,        const GLfixed *, m);
GL_APIFUNCTION_4(glRotatex,            GLfixed, angle, GLfixed, x, GLfixed, y, GLfixed, z);
GL_APIFUNCTION_3(glScalex,             GLfixed, x, GLfixed, y, GLfixed, z);
GL_APIFUNCTION_3(glTranslatex,         GLfixed, x, GLfixed, y, GLfixed, z);
GL_APIFUNCTION_6(glFrustumx,           GLfixed, left, GLfixed, right, GLfixed, bottom, GLfixed, top, GLfixed, zNear, GLfixed, zFar);
GL_APIFUNCTION_6(glOrthox,             GLfixed, left, GLfixed, right, GLfixed, bottom, GLfixed, top, GLfixed, zNear, GLfixed, zFar);
//GL_APIFUNCTION_2(glClipPlanex,         GLenum plane, const GLfixed *equation);
//GL_APIFUNCTION_2(glGetClipPlanex,      GLenum pname, GLfixed eqn[4]);
GL_APIFUNCTION_3(glMaterialx,          GLenum, face, GLenum, pname, GLfixed, param);
GL_APIFUNCTION_3(glMaterialxv,         GLenum, face, GLenum, pname, const GLfixed *, params);
//GL_APIFUNCTION_3(glGetMaterialxv,      GLenum face, GLenum pname, GLfixed *params);
GL_APIFUNCTION_3(glLightx,             GLenum, light, GLenum, pname, GLfixed, param);
GL_APIFUNCTION_3(glLightxv,            GLenum, light, GLenum, pname, const GLfixed *, params);
//GL_APIFUNCTION_3(glGetLightxv,         GLenum light, GLenum pname, GLfixed *params);
//GL_APIFUNCTION_2(glLightModelx,        GLenum pname, GLfixed param);
//GL_APIFUNCTION_2(glLightModelxv,       GLenum pname, const GLfixed *params);
//GL_APIFUNCTION_1(glPointSizex,         GLfixed size);
//GL_APIFUNCTION_2(glPointParameterx,    GLenum pname, GLfixed param);
//GL_APIFUNCTION_2(glPointParameterxv,   GLenum pname, const GLfixed *params);
//GL_APIFUNCTION_1(glLineWidthx,         GLfixed width);
//GL_APIFUNCTION_2(glPolygonOffsetx,     GLfixed factor, GLfixed units);
GL_APIFUNCTION_3(glTexParameterx,      GLenum, target, GLenum, pname, GLfixed, param);
//GL_APIFUNCTION_3(glTexParameterxv,     GLenum target, GLenum pname, const GLfixed *params);
//GL_APIFUNCTION_3(glTexEnvx,            GLenum target, GLenum pname, GLfixed param);
//GL_APIFUNCTION_3(glTexEnvxv,           GLenum target, GLenum pname, const GLfixed *params);
//GL_APIFUNCTION_3(glGetTexEnvxv,        GLenum env, GLenum pname, GLfixed *params);
//GL_APIFUNCTION_3(glGetTexParameterxv,  GLenum target, GLenum pname, GLfixed *params);
GL_APIFUNCTION_2(glFogx,               GLenum, pname, GLfixed, param);
GL_APIFUNCTION_2(glFogxv,              GLenum, pname, const GLfixed *, params);
//GL_APIFUNCTION_2(glSampleCoveragex,    GLclampx value, GLboolean invert);
//GL_APIFUNCTION_2(glAlphaFuncx,         GLenum func, GLclampx ref);
GL_APIFUNCTION_4(glClearColorx,        GLclampx, red, GLclampx, green, GLclampx, blue, GLclampx, alpha);
GL_APIFUNCTION_1(glClearDepthx,        GLclampx, depth);
//GL_APIFUNCTION_2(glGetFixedv,          GLenum pname, GLfixed *params);

//#endif // OPENGL_ES_COMMON_LITE

//#ifdef OPENGL_ES_COMMON

GL_APIFUNCTION_3(glNormal3f,           GLfloat, nx, GLfloat, ny, GLfloat, nz);
//GL_APIFUNCTION_5(glMultiTexCoord4f,    GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GL_APIFUNCTION_4(glColor4f,            GLfloat, red, GLfloat, green, GLfloat, blue, GLfloat, alpha);
GL_APIFUNCTION_2(glDepthRangef,        GLclampf, zNear, GLclampf, zFar);
GL_APIFUNCTION_1(glLoadMatrixf,        const GLfloat *, m);
GL_APIFUNCTION_1(glMultMatrixf,        const GLfloat *, m);
GL_APIFUNCTION_4(glRotatef,            GLfloat, angle, GLfloat, x, GLfloat, y, GLfloat, z);
GL_APIFUNCTION_3(glScalef,             GLfloat, x, GLfloat, y, GLfloat, z);
GL_APIFUNCTION_3(glTranslatef,         GLfloat, x, GLfloat, y, GLfloat, z);
GL_APIFUNCTION_6(glFrustumf,           GLfloat, left, GLfloat, right, GLfloat, bottom, GLfloat, top, GLfloat, zNear, GLfloat, zFar);
GL_APIFUNCTION_6(glOrthof,             GLfloat, left, GLfloat, right, GLfloat, bottom, GLfloat, top, GLfloat, zNear, GLfloat, zFar);
//GL_APIFUNCTION_2(glClipPlanef,         GLenum plane, const GLfloat *equation);
//GL_APIFUNCTION_2(glGetClipPlanef,      GLenum pname, GLfloat eqn[4]);
GL_APIFUNCTION_3(glMaterialf,          GLenum, face, GLenum, pname, GLfloat, param);
GL_APIFUNCTION_3(glMaterialfv,         GLenum, face, GLenum, pname, const GLfloat *, params);
//GL_APIFUNCTION_3(glGetMaterialfv,      GLenum face, GLenum pname, GLfloat *params);
GL_APIFUNCTION_3(glLightf,             GLenum, light, GLenum, pname, GLfloat, param);
GL_APIFUNCTION_3(glLightfv,            GLenum, light, GLenum, pname, const GLfloat *, params);
//GL_APIFUNCTION_3(glGetLightfv,         GLenum light, GLenum pname, GLfloat *params);
//GL_APIFUNCTION_2(glLightModelf,        GLenum pname, GLfloat param);
//GL_APIFUNCTION_2(glLightModelfv,       GLenum pname, const GLfloat *params);
//GL_APIFUNCTION_1(glPointSize,          GLfloat size);
//GL_APIFUNCTION_2(glPointParameterf,    GLenum pname, GLfloat param);
//GL_APIFUNCTION_2(glPointParameterfv,   GLenum pname, const GLfloat *params);
//GL_APIFUNCTION_1(glLineWidth,          GLfloat width);
//GL_APIFUNCTION_2(glPolygonOffset,      GLfloat factor, GLfloat units);
GL_APIFUNCTION_3(glTexParameterf,      GLenum, target, GLenum, pname, GLfloat, param);
//GL_APIFUNCTION_3(glTexParameterfv,     GLenum target, GLenum pname, const GLfloat *params);
//GL_APIFUNCTION_3(glTexEnvf,            GLenum target, GLenum pname, GLfloat param);
//GL_APIFUNCTION_3(glTexEnvfv,           GLenum target, GLenum pname, const GLfloat *params);
//GL_APIFUNCTION_3(glGetTexEnvfv,        GLenum env, GLenum pname, GLfloat *params);
//GL_APIFUNCTION_3(glGetTexParameterfv,  GLenum target, GLenum pname, GLfloat *params);
GL_APIFUNCTION_2(glFogf,               GLenum, pname, GLfloat, param);
GL_APIFUNCTION_2(glFogfv,              GLenum, pname, const GLfloat *, params);
//GL_APIFUNCTION_2(glSampleCoverage,     GLclampf value, GLboolean invert);
//GL_APIFUNCTION_2(glAlphaFunc,          GLenum func, GLclampf ref);
GL_APIFUNCTION_4(glClearColor,         GLclampf, red, GLclampf, green, GLclampf, blue, GLclampf, alpha);
GL_APIFUNCTION_1(glClearDepthf,        GLclampf, depth);
//GL_APIFUNCTION_2(glGetFloatv,          GLenum pname, GLfloat *params);

//#endif // OPENGL_ES_COMMON

//#endif // OPENGL_ES
