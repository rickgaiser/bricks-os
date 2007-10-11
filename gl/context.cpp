#include "context.h"
#include "fixedPoint.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  glClearColorx(gl_fpfromf(red), gl_fpfromf(green), gl_fpfromf(blue), gl_fpfromf(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glClearDepthf(GLclampf depth)
{
  glClearDepthx(gl_fpfromf(depth));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  glColor4x(gl_fpfromf(red), gl_fpfromf(green), gl_fpfromf(blue), gl_fpfromf(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glFogf(GLenum pname, GLfloat param)
{
  glFogx(pname, gl_fpfromf(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glFogfv(GLenum pname, const GLfloat * params)
{
  GLfixed xparams[] = {gl_fpfromf(params[0])
                     , gl_fpfromf(params[1])
                     , gl_fpfromf(params[2])
                     , gl_fpfromf(params[3])};

  glFogxv(pname, xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glLightf(GLenum light, GLenum pname, GLfloat param)
{
  glLightx(light, pname, gl_fpfromf(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
  GLfixed xparams[] = {gl_fpfromf(params[0])
                     , gl_fpfromf(params[1])
                     , gl_fpfromf(params[2])
                     , gl_fpfromf(params[3])};

  glLightxv(light, pname, xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
  glNormal3x(gl_fpfromf(nx), gl_fpfromf(ny), gl_fpfromf(nz));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  glRotatex(gl_fpfromf(angle), gl_fpfromf(x), gl_fpfromf(y), gl_fpfromf(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  glScalex(gl_fpfromf(x), gl_fpfromf(y), gl_fpfromf(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  glTranslatex(gl_fpfromf(x), gl_fpfromf(y), gl_fpfromf(z));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  glClearColor(gl_fptof(red), gl_fptof(green), gl_fptof(blue), gl_fptof(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glClearDepthx(GLclampx depth)
{
  glClearDepthf(gl_fptof(depth));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
  glColor4f(gl_fptof(red), gl_fptof(green), gl_fptof(blue), gl_fptof(alpha));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glFogx(GLenum pname, GLfixed param)
{
  glFogf(pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glFogxv(GLenum pname, const GLfixed *params)
{
  GLfloat fparams[] = {gl_fptof(params[0])
                     , gl_fptof(params[1])
                     , gl_fptof(params[2])
                     , gl_fptof(params[3])};

  glFogfv(pname, fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glLightx(GLenum light, GLenum pname, GLfixed param)
{
  glLightf(light, pname, gl_fptof(param));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
  GLfloat fparams[] = {gl_fptof(params[0])
                     , gl_fptof(params[1])
                     , gl_fptof(params[2])
                     , gl_fptof(params[3])};

  glLightfv(light, pname, fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  glNormal3f(gl_fptof(nx), gl_fptof(ny), gl_fptof(nz));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  glRotatef(gl_fptof(angle), gl_fptof(x), gl_fptof(y), gl_fptof(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  glScalef(gl_fptof(x), gl_fptof(y), gl_fptof(z));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  glTranslatef(gl_fptof(x), gl_fptof(y), gl_fptof(z));
}
