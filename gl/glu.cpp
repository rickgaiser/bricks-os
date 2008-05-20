#include "GL/glu.h"
#include "GLES/gl.h"
typedef unsigned int wint_t;
#include <math.h>


//-----------------------------------------------------------------------------
int
gluBuild2DMipmaps(GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, const void * data)
{
  return -1;
}

//-----------------------------------------------------------------------------
void
gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
  GLdouble xmin, xmax, ymin, ymax;

  ymax = zNear * tan(fovy * M_PI / 360.0f);
  ymin = -ymax;
  xmin = ymin * aspect;
  xmax = ymax * aspect;

  glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}

//-----------------------------------------------------------------------------
int
gluScaleImage(GLenum format, GLint widthin, GLint heightin, GLenum typein, const void * datain, GLint widthout, GLint heightout, GLenum typeout, void * dataout)
{
  return -1;
}
