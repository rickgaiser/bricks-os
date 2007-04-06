#include "GLES/gl.h"
#include "GLES/gl_extra.h"

#include "fixedPoint.h"
#include "context.h"

typedef unsigned int wint_t;
#include <math.h>


CContext context;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CEdge::CEdge(uint32_t height)
{
  x_ = new GLint[height];
  z_ = new fxp_zbuf_t[height];
  c_ = new SColor[height];
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
CEdge::add(SVertex * vfrom, SVertex * vto)
{
  if(vto->sy != vfrom->sy)
  {
    GLint dy(vto->sy - vfrom->sy);

    GLfixed x(fpfromi(16, vfrom->sx));
    GLfixed mx((fpfromi(16, vto->sx  - vfrom->sx )) / dy);

    fxp_zbuf_t z(vfrom->v[2]);
    fxp_zbuf_t mz((vto->v[2] - vfrom->v[2]) / dy);

    switch(context.shadingModel_)
    {
      case GL_FLAT:
      {
        int yfrom = (vfrom->sy < 0) ? 0 : (vfrom->sy);
        int yto   = (vto->sy >= context.viewportHeight) ? (context.viewportHeight - 1) : (vto->sy);
        for(int y(yfrom); y < yto; y++)
        {
          x_[y] = fptoi(16, x);
          z_[y] = z;

          x += mx;
          z += mz;
        }
        break;
      }
      case GL_SMOOTH:
      {
        GLfixed r(vfrom->cc.r);
        GLfixed g(vfrom->cc.g);
        GLfixed b(vfrom->cc.b);
        GLfixed mr((vto->cc.r - vfrom->cc.r) / dy);
        GLfixed mg((vto->cc.g - vfrom->cc.g) / dy);
        GLfixed mb((vto->cc.b - vfrom->cc.b) / dy);

        for(int y(vfrom->sy); y < vto->sy; y++)
        {
          if(y >= context.viewportHeight)
            break;

          if(y >= 0)
          {
            x_[y]   = fptoi(16, x);
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
void
glSetSurface(CSurface * surface)
{
  context.renderSurface = surface;
}

//-----------------------------------------------------------------------------
// GL API
//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  context.glClearColor(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClearDepthf(GLclampf depth)
{
  context.glClearDepthf(depth);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  context.glColor4f(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param)
{
  context.glLightf(light, pname, param);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
  context.glLightfv(light, pname, params);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
  context.glNormal3f(nx, ny, nz);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  context.glRotatef(angle, x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  context.glScalef(x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  context.glTranslatef(x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClear(GLbitfield mask)
{
  context.glClear(mask);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  context.glClearColorx(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glClearDepthx(GLclampx depth)
{
  context.glClearDepthx(depth);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
  context.glColor4ub(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  context.glColor4x(red, green, blue, alpha);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  context.glColorPointer(size, type, stride, pointer);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glDepthFunc(GLenum func)
{
  context.glDepthFunc(func);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glDisable(GLenum cap)
{
  context.glDisable(cap);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
  context.glDrawArrays(mode, first, count);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glEnable(GLenum cap)
{
  context.glEnable(cap);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glEnableClientState(GLenum array)
{
  context.glEnableClientState(array);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glFlush(void)
{
  context.glFlush();
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLightx(GLenum light, GLenum pname, GLfixed param)
{
  context.glLightx(light, pname, param);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed *params)
{
  context.glLightxv(light, pname, params);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glLoadIdentity()
{
  context.glLoadIdentity();
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glMatrixMode(GLenum mode)
{
  context.glMatrixMode(mode);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  context.glNormal3x(nx, ny, nz);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
  context.glNormalPointer(type, stride, pointer);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  context.glRotatex(angle, x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  context.glScalex(x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glShadeModel(GLenum mode)
{
  context.glShadeModel(mode);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  context.glTranslatex(x, y, z);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  context.glVertexPointer(size, type, stride, pointer);
}

//-----------------------------------------------------------------------------
GL_API void
GL_APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  context.glViewport(x, y, width, height);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// GLU API
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
  // Calculate field of view scalars
  context.fpFieldofviewYScalar = m_fpfromf(context.viewportHeight / tan(fovy));
  context.fpFieldofviewXScalar = m_fpmul(context.fpFieldofviewYScalar, m_fpfromf(aspect));
}
