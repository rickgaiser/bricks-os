#include "EGL/egl.h"
#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "fixedPoint.h"
#include "context.h"

typedef unsigned int wint_t;
#include <math.h>


extern void * eglGetCurrentGLESContext();

#define GLES_GET_CONTEXT(RETVAL) \
CContext * context = (CContext *)eglGetCurrentGLESContext(); \
if(context == 0) \
{ \
  return RETVAL; \
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CEdge::CEdge(uint32_t height)
 : iHeight_(height)
{
  x_ = new GLint[iHeight_];
  z_ = new GLfixed[iHeight_];
  c_ = new SColorFx[iHeight_];
}

//-----------------------------------------------------------------------------
CEdge::~CEdge()
{
  delete x_;
  delete z_;
  delete c_;
}

//-----------------------------------------------------------------------------
void
CEdge::add(SVertex * vfrom, SVertex * vto, GLenum shadingModel)
{
  if(vto->sy != vfrom->sy)
  {
    GLint dy(vto->sy - vfrom->sy);

    GLfixed x(gl_fpfromi(vfrom->sx));
    GLfixed mx((gl_fpfromi(vto->sx  - vfrom->sx )) / dy);

    GLfixed z(vfrom->v2[2]);
    GLfixed mz((vto->v2[2] - vfrom->v2[2]) / dy);

    switch(shadingModel)
    {
      case GL_FLAT:
      {
        int yfrom = (vfrom->sy < 0) ? 0 : (vfrom->sy);
        int yto   = (vto->sy >= iHeight_) ? (iHeight_ - 1) : (vto->sy);
        for(int y(yfrom); y < yto; y++)
        {
          x_[y] = gl_fptoi(x);
          z_[y] = z;

          x += mx;
          z += mz;
        }
        break;
      }
      case GL_SMOOTH:
      {
        GLfixed r(vfrom->c2.r);
        GLfixed g(vfrom->c2.g);
        GLfixed b(vfrom->c2.b);
        GLfixed mr((vto->c2.r - vfrom->c2.r) / dy);
        GLfixed mg((vto->c2.g - vfrom->c2.g) / dy);
        GLfixed mb((vto->c2.b - vfrom->c2.b) / dy);

        for(int y(vfrom->sy); y < vto->sy; y++)
        {
          if(y >= iHeight_)
            break;

          if(y >= 0)
          {
            x_[y]   = gl_fptoi(x);
            z_[y]   = z;
            c_[y].r = r;
            c_[y].g = g;
            c_[y].b = b;
          }

          x += mx;
          z += mz;
          r += mr;
          g += mg;
          b += mb;
        }
      }
      break;
    }
  }
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// GLU API
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
  GLES_GET_CONTEXT();
  // Calculate field of view scalars
  context->fpFieldofviewYScalar = gl_fpfromf(context->viewportHeight / tan(fovy));
  context->fpFieldofviewXScalar = gl_fpmul(context->fpFieldofviewYScalar, gl_fpfromf(aspect));

//  GLdouble sine, cotangent, deltaZ;
//  GLdouble radians = fovy / 2 * M_PI / 180;
//  deltaZ = zFar - zNear;
//  sine = sin(radians);
//  if((deltaZ == 0) || (sine == 0) || (aspect == 0))
//    return;
//  cotangent = cos(radians) / sine;

  context->matrixPerspective.loadIdentity();
//  context->matrixPerspective.matrix[0][0] = gl_fpfromf(cotangent / aspect);
//  context->matrixPerspective.matrix[1][1] = gl_fpfromf(cotangent);
//  context->matrixPerspective.matrix[2][2] = gl_fpfromf(-(zFar + zNear) / deltaZ);
//  context->matrixPerspective.matrix[2][3] = gl_fpfromi(-1);
//  context->matrixPerspective.matrix[3][2] = gl_fpfromf(-2 * zNear * zFar / deltaZ);
//  context->matrixPerspective.matrix[3][3] = gl_fpfromi(0);
}
