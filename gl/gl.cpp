#include "GL/gl.h"
#include "GLES/gl.h"
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
// Miscellaneous
GL_APIFUNCTION_4(glClearColor,         GLclampf, red, GLclampf, green, GLclampf, blue, GLclampf, alpha);
GL_APIFUNCTION_1(glClear,              GLbitfield, mask);
GL_APIFUNCTION_1(glCullFace,           GLenum, mode);
GL_APIFUNCTION_1(glFrontFace,          GLenum, mode);
GL_APIFUNCTION_1(glEnable,             GLenum, cap);
GL_APIFUNCTION_1(glDisable,            GLenum, cap);
GL_APIFUNCTION_1(glEnableClientState,  GLenum, array);
GL_APIFUNCTION_1(glDisableClientState, GLenum, array);
GL_API GLenum GL_APIENTRY glGetError (void){return pCurrentContext->glGetError();}
GL_APIFUNCTION_0(glFinish);
GL_APIFUNCTION_0(glFlush);
GL_APIFUNCTION_2(glHint,               GLenum, target, GLenum, mode);
// Depth Buffer
GL_APIFUNCTION_1(glClearDepthf,        GLclampf, depth);
GL_APIFUNCTION_1(glDepthFunc,          GLenum, func);
GL_APIFUNCTION_2(glDepthRangef,        GLclampf, zNear, GLclampf, zFar);
// Accumulation Buffer
// ...
// Transformation
GL_APIFUNCTION_1(glMatrixMode,         GLenum, mode);
GL_APIFUNCTION_6(glOrthof,             GLfloat, left, GLfloat, right, GLfloat, bottom, GLfloat, top, GLfloat, zNear, GLfloat, zFar);
GL_APIFUNCTION_6(glFrustumf,           GLfloat, left, GLfloat, right, GLfloat, bottom, GLfloat, top, GLfloat, zNear, GLfloat, zFar);
GL_APIFUNCTION_4(glViewport,           GLint, x, GLint, y, GLsizei, width, GLsizei, height);
GL_APIFUNCTION_0(glPushMatrix);
GL_APIFUNCTION_0(glPopMatrix);
GL_APIFUNCTION_0(glLoadIdentity);
GL_APIFUNCTION_1(glLoadMatrixf,        const GLfloat *, m);
GL_APIFUNCTION_1(glMultMatrixf,        const GLfloat *, m);
GL_APIFUNCTION_4(glRotatef,            GLfloat, angle, GLfloat, x, GLfloat, y, GLfloat, z);
GL_APIFUNCTION_3(glScalef,             GLfloat, x, GLfloat, y, GLfloat, z);
GL_APIFUNCTION_3(glTranslatef,         GLfloat, x, GLfloat, y, GLfloat, z);
// Display Lists
// ...
// Drawing Functions
GL_APIFUNCTION_1(glBegin,              GLenum, mode);
GL_APIFUNCTION_0(glEnd);
GL_APIFUNCTION_4(glVertex4f,           GLfloat, x, GLfloat, y, GLfloat, z, GLfloat, w);
GL_APIFUNCTION_3(glNormal3f,           GLfloat, nx, GLfloat, ny, GLfloat, nz);
GL_APIFUNCTION_4(glColor4f,            GLfloat, red, GLfloat, green, GLfloat, blue, GLfloat, alpha);
GL_APIFUNCTION_4(glTexCoord4f,         GLfloat, s, GLfloat, t, GLfloat, r, GLfloat, q);
// Vertex Arrays (OpenGL 1.1)
GL_APIFUNCTION_4(glVertexPointer,      GLint, size, GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_3(glNormalPointer,      GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_4(glColorPointer,       GLint, size, GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_4(glTexCoordPointer,    GLint, size, GLenum, type, GLsizei, stride, const GLvoid *, pointer);
GL_APIFUNCTION_3(glDrawArrays,         GLenum, mode, GLint, first, GLsizei, count);
// Lighting
GL_APIFUNCTION_1(glShadeModel,         GLenum, mode);
GL_APIFUNCTION_3(glLightf,             GLenum, light, GLenum, pname, GLfloat, param);
GL_APIFUNCTION_3(glLightfv,            GLenum, light, GLenum, pname, const GLfloat *, params);
GL_APIFUNCTION_3(glMaterialf,          GLenum, face, GLenum, pname, GLfloat, param);
GL_APIFUNCTION_3(glMaterialfv,         GLenum, face, GLenum, pname, const GLfloat *, params);
// Raster functions
// ...
// Stenciling
// ...
// Texture mapping
GL_APIFUNCTION_3(glTexParameterf,      GLenum, target, GLenum, pname, GLfloat, param);
GL_APIFUNCTION_9(glTexImage2D,         GLenum, target, GLint, level, GLint, internalformat, GLsizei, width, GLsizei, height, GLint, border, GLenum, format, GLenum, type, const GLvoid *, pixels);
// OpenGL 1.1 functions
GL_APIFUNCTION_2(glGenTextures,        GLsizei, n, GLuint *, textures);
GL_APIFUNCTION_2(glDeleteTextures,     GLsizei, n, const GLuint *, textures);
GL_APIFUNCTION_2(glBindTexture,        GLenum, target, GLuint, texture);
// Evaluators
// ...
// Fog
GL_APIFUNCTION_2(glFogf,               GLenum, pname, GLfloat, param);
GL_APIFUNCTION_2(glFogfv,              GLenum, pname, const GLfloat *, params);
// Selection and Feedback
// ...

// FIXED point
GL_APIFUNCTION_4(glClearColorx,        GLclampx, red, GLclampx, green, GLclampx, blue, GLclampx, alpha);
GL_APIFUNCTION_1(glClearDepthx,        GLclampx, depth);
GL_APIFUNCTION_2(glDepthRangex,        GLclampx, zNear, GLclampx, zFar);
GL_APIFUNCTION_6(glOrthox,             GLfixed, left, GLfixed, right, GLfixed, bottom, GLfixed, top, GLfixed, zNear, GLfixed, zFar);
GL_APIFUNCTION_6(glFrustumx,           GLfixed, left, GLfixed, right, GLfixed, bottom, GLfixed, top, GLfixed, zNear, GLfixed, zFar);
GL_APIFUNCTION_1(glLoadMatrixx,        const GLfixed *, m);
GL_APIFUNCTION_1(glMultMatrixx,        const GLfixed *, m);
GL_APIFUNCTION_4(glRotatex,            GLfixed, angle, GLfixed, x, GLfixed, y, GLfixed, z);
GL_APIFUNCTION_3(glScalex,             GLfixed, x, GLfixed, y, GLfixed, z);
GL_APIFUNCTION_3(glTranslatex,         GLfixed, x, GLfixed, y, GLfixed, z);
GL_APIFUNCTION_3(glNormal3x,           GLfixed, nx, GLfixed, ny, GLfixed, nz);
GL_APIFUNCTION_4(glColor4x,            GLfixed, red, GLfixed, green, GLfixed, blue, GLfixed, alpha);
GL_APIFUNCTION_3(glLightx,             GLenum, light, GLenum, pname, GLfixed, param);
GL_APIFUNCTION_3(glLightxv,            GLenum, light, GLenum, pname, const GLfixed *, params);
GL_APIFUNCTION_3(glMaterialx,          GLenum, face, GLenum, pname, GLfixed, param);
GL_APIFUNCTION_3(glMaterialxv,         GLenum, face, GLenum, pname, const GLfixed *, params);
GL_APIFUNCTION_3(glTexParameterx,      GLenum, target, GLenum, pname, GLfixed, param);
GL_APIFUNCTION_2(glFogx,               GLenum, pname, GLfixed, param);
GL_APIFUNCTION_2(glFogxv,              GLenum, pname, const GLfixed *, params);

// Wrappers
GL_API void GL_APIENTRY glClearDepth(GLclampd depth){
  glClearDepthf(depth);}
GL_API void GL_APIENTRY glDepthRange(GLclampd near_val, GLclampd far_val){
  glDepthRangef(near_val, far_val);}
GL_API void GL_APIENTRY glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val){
  glOrthof(left, right, bottom, top, near_val, far_val);}
GL_API void GL_APIENTRY glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val){
  glFrustumf(left, right, bottom, top, near_val, far_val);}

// Wrapper macro's
#define VERTEX_WRAPPERS(ch,type,def3,def4) \
  GL_API void GL_APIENTRY glVertex2##ch     (type x, type y)        {glVertex4##ch(x,    y,    def3, def4);} \
  GL_API void GL_APIENTRY glVertex3##ch     (type x, type y, type z){glVertex4##ch(x,    y,    z,    def4);} \
  GL_API void GL_APIENTRY glVertex2##ch##v  (const type * v)        {glVertex4##ch(v[0], v[1], def3, def4);} \
  GL_API void GL_APIENTRY glVertex3##ch##v  (const type * v)        {glVertex4##ch(v[0], v[1], v[2], def4);} \
  GL_API void GL_APIENTRY glVertex4##ch##v  (const type * v)        {glVertex4##ch(v[0], v[1], v[2], v[3]);}
#define NORMAL_WRAPPERS(ch,type) \
  GL_API void GL_APIENTRY glNormal##ch##fv  (const type * v){glNormal3##ch(v[0], v[1], v[2]);}
#define COLOR_WRAPPERS(ch,type,def4) \
  GL_API void GL_APIENTRY glColor3##ch      (type red, type green, type blue){glColor4##ch(red,  green, blue, def4);} \
  GL_API void GL_APIENTRY glColor3##ch##v   (const type * v)                 {glColor4##ch(v[0], v[1],  v[2], def4);} \
  GL_API void GL_APIENTRY glColor4##ch##v   (const type * v)                 {glColor4##ch(v[0], v[1],  v[2], v[3]);}
#define TEXCOORD_WRAPPERS(ch,type,def2,def3,def4) \
  GL_API void GL_APIENTRY glTexCoord1f (type s)                {glTexCoord4##ch(s,    def2, def3, def4);} \
  GL_API void GL_APIENTRY glTexCoord2f (type s, type t)        {glTexCoord4##ch(s,    t,    def3, def4);} \
  GL_API void GL_APIENTRY glTexCoord3f (type s, type t, type r){glTexCoord4##ch(s,    t,    r,    def4);} \
  GL_API void GL_APIENTRY glTexCoord1fv(const type * v)        {glTexCoord4##ch(v[0], def2, def3, def4);} \
  GL_API void GL_APIENTRY glTexCoord2fv(const type * v)        {glTexCoord4##ch(v[0], v[1], def3, def4);} \
  GL_API void GL_APIENTRY glTexCoord3fv(const type * v)        {glTexCoord4##ch(v[0], v[1], v[2], def4);} \
  GL_API void GL_APIENTRY glTexCoord4fv(const type * v)        {glTexCoord4##ch(v[0], v[1], v[2], v[3]);}

// Wrappers
//VERTEX_WRAPPERS(d, GLdouble, 0.0f, 1.0f);
VERTEX_WRAPPERS(f, GLfloat,  0.0f, 1.0f);
//VERTEX_WRAPPERS(i, GLint,    ???);
//VERTEX_WRAPPERS(s, GLshort,  ???);
//NORMAL_WRAPPERS(b, GLbyte  );
//NORMAL_WRAPPERS(d, GLdouble);
NORMAL_WRAPPERS(f, GLfloat );
//NORMAL_WRAPPERS(i, GLint   );
//NORMAL_WRAPPERS(s, GLshort );
//COLOR_WRAPPERS(b,  GLbyte,   ???);
//COLOR_WRAPPERS(d,  GLdouble, 1.0f);
COLOR_WRAPPERS(f,  GLfloat,  1.0f);
//COLOR_WRAPPERS(i,  GLint,    ???);
//COLOR_WRAPPERS(s,  GLshort,  ???);
//COLOR_WRAPPERS(ub, GLubyte,  ???);
//COLOR_WRAPPERS(ui, GLuint,   ???);
//COLOR_WRAPPERS(us, GLushort, ???);
//TEXCOORD_WRAPPERS(d, GLdouble, 0.0f, 0.0f, 1.0f);
TEXCOORD_WRAPPERS(f, GLfloat,  0.0f, 0.0f, 1.0f);
//TEXCOORD_WRAPPERS(i, GLfloat,  ???);
//TEXCOORD_WRAPPERS(s, GLfloat,  ???);
