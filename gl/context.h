#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H


#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "kernel/videoManager.h"
#include "matrix.h"
#include "fixedPoint.h"
#include "context.h"


// GLfixed fixed point math:
#define FP_PRESICION_GL 16
#define gl_fpfromi(i)   fpfromi(FP_PRESICION_GL,i)
#define gl_fptoi(i)     fptoi(FP_PRESICION_GL,i)
#define gl_fpfromf(i)   fpfromf(FP_PRESICION_GL,i)
#define gl_fptof(i)     fptof(FP_PRESICION_GL,i)
#define gl_fpmul(i1,i2) fpmul32(FP_PRESICION_GL,i1,i2)
#define gl_fpdiv(i1,i2) fpdiv32(FP_PRESICION_GL,i1,i2)
#define gl_fpclamp(i)   (i < 0 ? 0 : (i > gl_fpfromi(1) ? gl_fpfromi(1) : i))


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
typedef union
{
  struct
  {
    GLfixed x, y;
  };
  GLfixed v[2];
}SVector2, SVertex2;

//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfixed x, y, z;
  };
  GLfixed v[3];
}SVector3, SVertex3;

//-----------------------------------------------------------------------------
typedef union
{
  struct
  {
    GLfixed x, y, z, w;
  };
  GLfixed v[4];
}SVector4, SVertex4;

//-----------------------------------------------------------------------------
struct SVertex
{
  // Vertex itself
  // Original
  union
  {
    struct
    {
      GLfixed vx1, vy1, vz1, vw1;
    };
    GLfixed v1[4];
  };
  // Transformed
  union
  {
    struct
    {
      GLfixed vx2, vy2, vz2, vw2;
    };
    GLfixed v2[4];
  };

  // Normal vector
  // Original
  union
  {
    struct
    {
      GLfixed nx1, ny1, nz1, nw1;
    };
    GLfixed n1[4];
  };
  // Transformed
  union
  {
    struct
    {
      GLfixed nx2, ny2, nz2, nw2;
    };
    GLfixed n2[4];
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
  SColorFx  c1;
  // Lighted
  SColorFx  c2;

  // State: indicates if the xxx2 values have been created already
  bool      bProcessed;
};

//-----------------------------------------------------------------------------
// Triangle/Polygon
struct SPolygon
{
  SVertex * v[3];
};

//-----------------------------------------------------------------------------
class CEdge
{
public:
  CEdge(uint32_t height);
  ~CEdge();

  void add(SVertex * vfrom, SVertex * vto, GLenum shadingModel);

public:
  // Edge x
  GLint * x_;
  // Edge depth
  GLfixed * z_;
  // Edge color
  SColorFx * c_;

  int32_t iHeight_;
};

//-----------------------------------------------------------------------------
struct SBufferPointer
{
  GLint size;
  GLenum type;
  GLsizei stride;
  const GLvoid * pointer;
};

//-----------------------------------------------------------------------------
struct SLight
{
  SColorFx diffuse;
  SColorFx ambient;
  SColorFx specular;
  bool enabled;
};

//-----------------------------------------------------------------------------
class IGLESContext
{
public:
  virtual ~IGLESContext(){}

//  virtual void glAlphaFunc(GLenum func, GLclampf ref) = 0;
  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;
  virtual void glClearDepthf(GLclampf depth) = 0;
//  virtual void glClipPlanef(GLenum plane, const GLfloat *equation) = 0;
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = 0;
//  virtual void glDepthRangef(GLclampf zNear, GLclampf zFar) = 0;
  virtual void glFogf(GLenum pname, GLfloat param) = 0;
  virtual void glFogfv(GLenum pname, const GLfloat *params) = 0;
//  virtual void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) = 0;
//  virtual void glGetClipPlanef(GLenum pname, GLfloat eqn[4]) = 0;
//  virtual void glGetFloatv(GLenum pname, GLfloat *params) = 0;
//  virtual void glGetLightfv(GLenum light, GLenum pname, GLfloat *params) = 0;
//  virtual void glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params) = 0;
//  virtual void glGetTexEnvfv(GLenum env, GLenum pname, GLfloat *params) = 0;
//  virtual void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params) = 0;
//  virtual void glLightModelf(GLenum pname, GLfloat param) = 0;
//  virtual void glLightModelfv(GLenum pname, const GLfloat *params) = 0;
  virtual void glLightf(GLenum light, GLenum pname, GLfloat param) = 0;
  virtual void glLightfv(GLenum light, GLenum pname, const GLfloat * params) = 0;
//  virtual void glLineWidth(GLfloat width) = 0;
//  virtual void glLoadMatrixf(const GLfloat *m) = 0;
//  virtual void glMaterialf(GLenum face, GLenum pname, GLfloat param) = 0;
//  virtual void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params) = 0;
//  virtual void glMultMatrixf(const GLfloat *m) = 0;
//  virtual void glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) = 0;
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz) = 0;
//  virtual void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) = 0;
//  virtual void glPointParameterf(GLenum pname, GLfloat param) = 0;
//  virtual void glPointParameterfv(GLenum pname, const GLfloat *params) = 0;
//  virtual void glPointSize(GLfloat size) = 0;
//  virtual void glPolygonOffset(GLfloat factor, GLfloat units) = 0;
  virtual void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) = 0;
  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z) = 0;
//  virtual void glTexEnvf(GLenum target, GLenum pname, GLfloat param) = 0;
//  virtual void glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params) = 0;
//  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param) = 0;
//  virtual void glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params) = 0;
  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z) = 0;

//  virtual void glActiveTexture(GLenum texture) = 0;
//  virtual void glAlphaFuncx(GLenum func, GLclampx ref) = 0;
//  virtual void glBindBuffer(GLenum target, GLuint buffer) = 0;
//  virtual void glBindTexture(GLenum target, GLuint texture) = 0;
//  virtual void glBlendFunc(GLenum sfactor, GLenum dfactor) = 0;
//  virtual void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) = 0;
//  virtual void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data) = 0;
  virtual void glClear(GLbitfield mask) = 0;
  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha) = 0;
  virtual void glClearDepthx(GLclampx depth) = 0;
//  virtual void glClearStencil(GLint s) = 0;
//  virtual void glClientActiveTexture(GLenum texture) = 0;
//  virtual void glClipPlanex(GLenum plane, const GLfixed *equation) = 0;
  virtual void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) = 0;
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) = 0;
//  virtual void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) = 0;
  virtual void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer) = 0;
//  virtual void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) = 0;
//  virtual void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) = 0;
//  virtual void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) = 0;
//  virtual void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual void glCullFace(GLenum mode) = 0;
//  virtual void glDeleteBuffers(GLsizei n, const GLuint *buffers) = 0;
//  virtual void glDeleteTextures(GLsizei n, const GLuint *textures) = 0;
  virtual void glDepthFunc(GLenum func) = 0;
//  virtual void glDepthMask(GLboolean flag) = 0;
//  virtual void glDepthRangex(GLclampx zNear, GLclampx zFar) = 0;
  virtual void glDisable(GLenum cap) = 0;
  virtual void glDisableClientState(GLenum array) = 0;
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count) = 0;
//  virtual void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) = 0;
  virtual void glEnable(GLenum cap) = 0;
  virtual void glEnableClientState(GLenum array) = 0;
//  virtual void glFinish(void) = 0;
  virtual void glFlush(void) = 0;
  virtual void glFogx(GLenum pname, GLfixed param) = 0;
  virtual void glFogxv(GLenum pname, const GLfixed *params) = 0;
//  virtual void glFrontFace(GLenum mode) = 0;
//  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) = 0;
//  virtual void glGetBooleanv(GLenum pname, GLboolean *params) = 0;
//  virtual void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params) = 0;
//  virtual void glGetClipPlanex(GLenum pname, GLfixed eqn[4]) = 0;
//  virtual void glGenBuffers(GLsizei n, GLuint *buffers) = 0;
//  virtual void glGenTextures(GLsizei n, GLuint *textures) = 0;
//  virtual GLenum glGetError(void) = 0;
//  virtual void glGetFixedv(GLenum pname, GLfixed *params) = 0;
//  virtual void glGetIntegerv(GLenum pname, GLint *params) = 0;
//  virtual void glGetLightxv(GLenum light, GLenum pname, GLfixed *params) = 0;
//  virtual void glGetMaterialxv(GLenum face, GLenum pname, GLfixed *params) = 0;
//  virtual void glGetPointerv(GLenum pname, void **params) = 0;
//  virtual const GLubyte * glGetString(GLenum name) = 0;
//  virtual void glGetTexEnviv(GLenum env, GLenum pname, GLint *params) = 0;
//  virtual void glGetTexEnvxv(GLenum env, GLenum pname, GLfixed *params) = 0;
//  virtual void glGetTexParameteriv(GLenum target, GLenum pname, GLint *params) = 0;
//  virtual void glGetTexParameterxv(GLenum target, GLenum pname, GLfixed *params) = 0;
//  virtual void glHint(GLenum target, GLenum mode) = 0;
//  virtual GLboolean glIsBuffer(GLuint buffer) = 0;
//  virtual GLboolean glIsEnabled(GLenum cap) = 0;
//  virtual GLboolean glIsTexture(GLuint texture) = 0;
//  virtual void glLightModelx(GLenum pname, GLfixed param) = 0;
//  virtual void glLightModelxv(GLenum pname, const GLfixed *params) = 0;
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param) = 0;
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params) = 0;
//  virtual void glLineWidthx(GLfixed width) = 0;
  virtual void glLoadIdentity(void) = 0;
//  virtual void glLoadMatrixx(const GLfixed *m) = 0;
//  virtual void glLogicOp(GLenum opcode) = 0;
//  virtual void glMaterialx(GLenum face, GLenum pname, GLfixed param) = 0;
//  virtual void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params) = 0;
  virtual void glMatrixMode(GLenum mode) = 0;
//  virtual void glMultMatrixx(const GLfixed *m) = 0;
//  virtual void glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q) = 0;
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz) = 0;
  virtual void glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer) = 0;
//  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) = 0;
//  virtual void glPixelStorei(GLenum pname, GLint param) = 0;
//  virtual void glPointParameterx(GLenum pname, GLfixed param) = 0;
//  virtual void glPointParameterxv(GLenum pname, const GLfixed *params) = 0;
//  virtual void glPointSizex(GLfixed size) = 0;
//  virtual void glPolygonOffsetx(GLfixed factor, GLfixed units) = 0;
//  virtual void glPopMatrix(void) = 0;
//  virtual void glPushMatrix(void) = 0;
//  virtual void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) = 0;
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z) = 0;
//  virtual void glSampleCoverage(GLclampf value, GLboolean invert) = 0;
//  virtual void glSampleCoveragex(GLclampx value, GLboolean invert) = 0;
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z) = 0;
//  virtual void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual void glShadeModel(GLenum mode) = 0;
//  virtual void glStencilFunc(GLenum func, GLint ref, GLuint mask) = 0;
//  virtual void glStencilMask(GLuint mask) = 0;
//  virtual void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) = 0;
//  virtual void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) = 0;
//  virtual void glTexEnvi(GLenum target, GLenum pname, GLint param) = 0;
//  virtual void glTexEnvx(GLenum target, GLenum pname, GLfixed param) = 0;
//  virtual void glTexEnviv(GLenum target, GLenum pname, const GLint *params) = 0;
//  virtual void glTexEnvxv(GLenum target, GLenum pname, const GLfixed *params) = 0;
//  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) = 0;
//  virtual void glTexParameteri(GLenum target, GLenum pname, GLint param) = 0;
//  virtual void glTexParameterx(GLenum target, GLenum pname, GLfixed param) = 0;
//  virtual void glTexParameteriv(GLenum target, GLenum pname, const GLint *params) = 0;
//  virtual void glTexParameterxv(GLenum target, GLenum pname, const GLfixed *params) = 0;
//  virtual void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) = 0;
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z) = 0;
  virtual void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer) = 0;
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
};

//-----------------------------------------------------------------------------
class CAGLESFloatToFxContext
 : public IGLESContext
{
public:
  virtual ~CAGLESFloatToFxContext(){}

  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  virtual void glClearDepthf(GLclampf depth);
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  virtual void glFogf(GLenum pname, GLfloat param);
  virtual void glFogfv(GLenum pname, const GLfloat *params);
  virtual void glLightf(GLenum light, GLenum pname, GLfloat param);
  virtual void glLightfv(GLenum light, GLenum pname, const GLfloat * params);
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
  virtual void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z);
  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z);
};

//-----------------------------------------------------------------------------
class CAGLESFxToFloatContext
 : public IGLESContext
{
public:
  virtual ~CAGLESFxToFloatContext(){}

  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);
};

//-----------------------------------------------------------------------------
class CGLESFxContext
 : public CAGLESFloatToFxContext
{
public:
  friend void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

  CGLESFxContext();
  virtual ~CGLESFxContext();

  void setSurface(CSurface * surface);
  CSurface * getSurface();

  virtual void glClear(GLbitfield mask);
  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
  virtual void glClearDepthx(GLclampx depth);
  virtual void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
  virtual void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glCullFace(GLenum mode);
  virtual void glDepthFunc(GLenum func);
  virtual void glDisable(GLenum cap);
  virtual void glDisableClientState(GLenum array);
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count);
  virtual void glEnable(GLenum cap);
  virtual void glEnableClientState(GLenum array);
  virtual void glFlush(void);
  virtual void glFogx(GLenum pname, GLfixed param);
  virtual void glFogxv(GLenum pname, const GLfixed *params);
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param);
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params);
  virtual void glLoadIdentity(void);
  virtual void glMatrixMode(GLenum mode);
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
  virtual void glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glShadeModel(GLenum mode);
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z);
  virtual void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

private:
  void hline(CEdge & from, CEdge & to, GLint & y, SColorFx c);
  void hline_s(CEdge & from, CEdge & to, GLint & y);
  void plotPoly(SPolygon & poly);

private:
  CSurface * renderSurface;
  GLfixed * zbuffer;

  GLenum    shadingModel_;
  bool      cullFaceEnabled_;
  GLenum    cullFaceMode_;

  // Matrix
  GLenum    matrixMode_;
  CMatrixFx   matrixModelView;
  CMatrixFx   matrixProjection;
  CMatrixFx   matrixPerspective;
  CMatrixFx   matrixRotation;
  CMatrixFx * pCurrentMatrix_;

  // Buffers
  SBufferPointer bufVertex_;
  bool           bBufVertexEnabled_;
  SBufferPointer bufNormal_;
  bool           bBufNormalEnabled_;
  SBufferPointer bufColor_;
  bool           bBufColorEnabled_;

  // Colors
  SColorFx  clCurrent;
  SColorFx  clClear;

  // Lighting
  bool      lightingEnabled_;
  SLight    lights_[8];

  // Normals
  GLfixed   normal_[4];

  // Fog
  bool      fogEnabled_;
  GLfixed   fogDensity_;
  GLfixed   fogStart_;
  GLfixed   fogEnd_;
  SColorFx  fogColor_;

  // Depth testing
  bool      depthTestEnabled_;
  GLenum    depthFunction_;
  GLfixed   depthClear_;

  CEdge   * edge1;
  CEdge   * edge2;

  // Viewport
  GLint     viewportXOffset;
  GLint     viewportYOffset;
  GLsizei   viewportPixelCount;
  GLsizei   viewportByteCount;
  GLsizei   viewportWidth;
  GLsizei   viewportHeight;
  GLfixed   fpFieldofviewXScalar;
  GLfixed   fpFieldofviewYScalar;
};


#endif // GL_CONTEXT_H
