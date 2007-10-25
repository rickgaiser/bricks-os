#include "context.h"
#include "fixedPoint.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CAGLESBuffers::CAGLESBuffers()
 : bBufColorEnabled_(false)
 , bBufNormalEnabled_(false)
 , bBufTexCoordEnabled_(false)
 , bBufVertexEnabled_(false)
{
}

//-----------------------------------------------------------------------------
CAGLESBuffers::~CAGLESBuffers()
{
}

//-----------------------------------------------------------------------------
void
CAGLESBuffers::glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufColor_.size    = size;
  bufColor_.type    = type;
  bufColor_.stride  = stride;
  bufColor_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CAGLESBuffers::glDisableClientState(GLenum array)
{
  switch(array)
  {
    case GL_VERTEX_ARRAY: bBufVertexEnabled_ = false; break;
    case GL_NORMAL_ARRAY: bBufNormalEnabled_ = false; break;
    case GL_COLOR_ARRAY:  bBufColorEnabled_  = false; break;

    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CAGLESBuffers::glEnableClientState(GLenum array)
{
  switch(array)
  {
    case GL_COLOR_ARRAY:         bBufColorEnabled_    = true; break;
    case GL_NORMAL_ARRAY:        bBufNormalEnabled_   = true; break;
    case GL_TEXTURE_COORD_ARRAY: bBufTexCoordEnabled_ = true; break;
    case GL_VERTEX_ARRAY:        bBufVertexEnabled_   = true; break;
    default:
      ; // Not supported
  };
}

//-----------------------------------------------------------------------------
void
CAGLESBuffers::glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufNormal_.size    = 3;  // x,y,z
  bufNormal_.type    = type;
  bufNormal_.stride  = stride;
  bufNormal_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CAGLESBuffers::glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufTexCoord_.size    = size;
  bufTexCoord_.type    = type;
  bufTexCoord_.stride  = stride;
  bufTexCoord_.pointer = pointer;
}

//-----------------------------------------------------------------------------
void
CAGLESBuffers::glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
  bufVertex_.size    = size;
  bufVertex_.type    = type;
  bufVertex_.stride  = stride;
  bufVertex_.pointer = pointer;
}

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
CAGLESFloatToFxContext::glLoadMatrixf(const GLfloat *m)
{
  GLfixed xparams[] = {gl_fpfromf(m[0])
                     , gl_fpfromf(m[1])
                     , gl_fpfromf(m[2])
                     , gl_fpfromf(m[3])
                     , gl_fpfromf(m[4])
                     , gl_fpfromf(m[5])
                     , gl_fpfromf(m[6])
                     , gl_fpfromf(m[7])
                     , gl_fpfromf(m[8])
                     , gl_fpfromf(m[9])
                     , gl_fpfromf(m[10])
                     , gl_fpfromf(m[11])
                     , gl_fpfromf(m[12])
                     , gl_fpfromf(m[13])
                     , gl_fpfromf(m[14])
                     , gl_fpfromf(m[15])};

  glLoadMatrixx(xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glMultMatrixf(const GLfloat *m)
{
  GLfixed xparams[] = {gl_fpfromf(m[0])
                     , gl_fpfromf(m[1])
                     , gl_fpfromf(m[2])
                     , gl_fpfromf(m[3])
                     , gl_fpfromf(m[4])
                     , gl_fpfromf(m[5])
                     , gl_fpfromf(m[6])
                     , gl_fpfromf(m[7])
                     , gl_fpfromf(m[8])
                     , gl_fpfromf(m[9])
                     , gl_fpfromf(m[10])
                     , gl_fpfromf(m[11])
                     , gl_fpfromf(m[12])
                     , gl_fpfromf(m[13])
                     , gl_fpfromf(m[14])
                     , gl_fpfromf(m[15])};

  glMultMatrixx(xparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFloatToFxContext::glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  glFrustumx(gl_fpfromf(left), gl_fpfromf(right), gl_fpfromf(bottom), gl_fpfromf(top), gl_fpfromf(zNear), gl_fpfromf(zFar));
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
CAGLESFloatToFxContext::glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  glOrthox(gl_fpfromf(left), gl_fpfromf(right), gl_fpfromf(bottom), gl_fpfromf(top), gl_fpfromf(zNear), gl_fpfromf(zFar));
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
CAGLESFxToFloatContext::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  glFrustumf(gl_fptof(left), gl_fptof(right), gl_fptof(bottom), gl_fptof(top), gl_fptof(zNear), gl_fptof(zFar));
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
CAGLESFxToFloatContext::glLoadMatrixx(const GLfixed *m)
{
  GLfloat fparams[] = {gl_fptof(m[0])
                     , gl_fptof(m[1])
                     , gl_fptof(m[2])
                     , gl_fptof(m[3])
                     , gl_fptof(m[4])
                     , gl_fptof(m[5])
                     , gl_fptof(m[6])
                     , gl_fptof(m[7])
                     , gl_fptof(m[8])
                     , gl_fptof(m[9])
                     , gl_fptof(m[10])
                     , gl_fptof(m[11])
                     , gl_fptof(m[12])
                     , gl_fptof(m[13])
                     , gl_fptof(m[14])
                     , gl_fptof(m[15])};

  glLoadMatrixf(fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glMultMatrixx(const GLfixed *m)
{
  GLfloat fparams[] = {gl_fptof(m[0])
                     , gl_fptof(m[1])
                     , gl_fptof(m[2])
                     , gl_fptof(m[3])
                     , gl_fptof(m[4])
                     , gl_fptof(m[5])
                     , gl_fptof(m[6])
                     , gl_fptof(m[7])
                     , gl_fptof(m[8])
                     , gl_fptof(m[9])
                     , gl_fptof(m[10])
                     , gl_fptof(m[11])
                     , gl_fptof(m[12])
                     , gl_fptof(m[13])
                     , gl_fptof(m[14])
                     , gl_fptof(m[15])};

  glMultMatrixf(fparams);
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
  glNormal3f(gl_fptof(nx), gl_fptof(ny), gl_fptof(nz));
}

//-----------------------------------------------------------------------------
void
CAGLESFxToFloatContext::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  glOrthof(gl_fptof(left), gl_fptof(right), gl_fptof(bottom), gl_fptof(top), gl_fptof(zNear), gl_fptof(zFar));
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
