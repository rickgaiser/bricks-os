#include "GL/glu.h"
#include "GLES/gl.h"
typedef unsigned int wint_t;
#include <math.h>


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
