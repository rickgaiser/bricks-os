#include "glesContextNDS.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
typedef unsigned int wint_t;
#include <math.h>


#define fpRGB(r,g,b) (0x8000 | \
                      (((b*255) >>  9) & 0x7c00) | \
                      (((g*255) >> 14) & 0x03e0) | \
                      (((r*255) >> 19) & 0x001f))


//-----------------------------------------------------------------------------
// glPolyFmt constants
//-----------------------------------------------------------------------------
#define POLY_ALPHA(n)           ((n) << 16)
#define POLY_TOON_SHADING       0x20
#define POLY_CULL_BACK          0x80
#define POLY_CULL_FRONT         0x40
#define POLY_CULL_NONE          0xC0
#define POLY_ID(n)              ((n)<<24)

#define POLY_FORMAT_LIGHT0      0x1
#define POLY_FORMAT_LIGHT1      0x2
#define POLY_FORMAT_LIGHT2      0x4
#define POLY_FORMAT_LIGHT3      0x8

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CNDSGLESContext::CNDSGLESContext()
 : CSoftGLESFixed()
{
  REG_POWCNT |= POWER_LCD |POWER_2D_TOP |POWER_2D_BOTTOM | POWER_3D_CORE | POWER_3D_MATRIX;

  REG_DISPCNT = MODE_0 | BG0_ENABLE | ENABLE_3D;

  GFX_POLY_FORMAT = POLY_ALPHA(31) | POLY_CULL_NONE;

  GFX_CLEAR_DEPTH = 0x7fff;
}

//-----------------------------------------------------------------------------
CNDSGLESContext::~CNDSGLESContext()
{
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClear(GLbitfield mask)
{
  if(mask & GL_COLOR_BUFFER_BIT)
  {
  }

  if(mask & GL_DEPTH_BUFFER_BIT)
  {
  }
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
  BG_PALETTE[0] = fpRGB(red, green, blue);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFlush(void)
{
  GFX_FLUSH = 2;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  zNear_ = zNear;
  zFar_  = zFar;
  zA_    = -gl_fpdiv((zFar + zNear), (zFar - zNear)) >> 4;
  zB_    = -gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear)) >> 4;

  MATRIX_MULT4x4 = gl_fpdiv((zNear << 1), (right - left)) >> 4;
  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;
  MATRIX_MULT4x4 = gl_fpdiv((right + left), (right - left)) >> 4;
  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;

  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;
  MATRIX_MULT4x4 = gl_fpdiv((zNear << 1), (top - bottom)) >> 4;
  MATRIX_MULT4x4 = gl_fpdiv((top + bottom), (top - bottom)) >> 4;
  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;

  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;
  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;
  MATRIX_MULT4x4 = zA_;
  MATRIX_MULT4x4 = gl_fpfromi(-1) >> 4;

  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;
  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;
  MATRIX_MULT4x4 = zB_;
  MATRIX_MULT4x4 = gl_fpfromi(0) >> 4;

  MATRIX_STORE = 0;  // GL_PROJECTION
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glLoadIdentity()
{
  MATRIX_IDENTITY = 0;
}

//-----------------------------------------------------------------------------
//#define GL_PROJECTION      0
//#define GL_POSITION        1
//#define GL_MODELVIEW       2
//#define GL_TEXTURE         3
void
CNDSGLESContext::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_MODELVIEW:  MATRIX_CONTROL = 2; break;
    case GL_PROJECTION: MATRIX_CONTROL = 0; break;
  };
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  /*
  int32_t axis[3];
  int32_t sine = SIN[angle &  LUT_MASK];
  int32_t cosine = COS[angle & LUT_MASK];
  int32_t one_minus_cosine = inttof32(1) - cosine;

  axis[0]=x;
  axis[1]=y;
  axis[2]=z;

  normalizef32(axis);   // should require passed in normalized?

  MATRIX_MULT3x3 = cosine + mulf32(one_minus_cosine, mulf32(axis[0], axis[0]));
  MATRIX_MULT3x3 = mulf32(one_minus_cosine, mulf32(axis[0], axis[1])) - mulf32(axis[2], sine);
  MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) + mulf32(axis[1], sine);

  MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]),  axis[1]) + mulf32(axis[2], sine);
  MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[1]), axis[1]);
  MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) - mulf32(axis[0], sine);

  MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[0]), axis[2]) - mulf32(axis[1], sine);
  MATRIX_MULT3x3 = mulf32(mulf32(one_minus_cosine, axis[1]), axis[2]) + mulf32(axis[0], sine);
  MATRIX_MULT3x3 = cosine + mulf32(mulf32(one_minus_cosine, axis[2]), axis[2]);
  */

  uint32_t iSin = gl_fpfromf(sin(gl_fptof(angle) * M_PI / 180.0f)) >> 4;
  uint32_t iCos = gl_fpfromf(cos(gl_fptof(angle) * M_PI / 180.0f)) >> 4;

  MATRIX_MULT3x3 = iCos;
  MATRIX_MULT3x3 = 0;
  MATRIX_MULT3x3 = iSin;

  MATRIX_MULT3x3 = 0;
  MATRIX_MULT3x3 = gl_fpfromi(1) >> 4;
  MATRIX_MULT3x3 = 0;

  MATRIX_MULT3x3 = -iSin;
  MATRIX_MULT3x3 = 0;
  MATRIX_MULT3x3 = iCos;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_SCALE = (x >> 4);
  MATRIX_SCALE = (y >> 4);
  MATRIX_SCALE = (z >> 4);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_TRANSLATE = (x >> 4);
  MATRIX_TRANSLATE = (y >> 4);
  MATRIX_TRANSLATE = (z >> 4);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::rasterPoly(SPolygonFx & poly)
{
  GFX_BEGIN = 0;//GL_TRIANGLE;
  for(int i(0); i < 3; i++)
  {
    GFX_COLOR = fpRGB(poly.v[i]->c1.r, poly.v[i]->c1.g, poly.v[i]->c1.b);
    // Fixed Point 4.12
    GFX_VERTEX16 = ((poly.v[i]->v1[1] << 12) & 0xffff0000) | ((poly.v[i]->v1[0] >> 4) & 0xffff);
    GFX_VERTEX16 = (poly.v[i]->v1[2] >> 4) & 0xffff;
  }
  GFX_END = 0;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  GFX_VIEWPORT = x + (y << 8) + ((width-1) << 16) + ((height-1) << 24);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IGLESContext *
getGLESContext()
{
  return new CNDSGLESContext;
}
