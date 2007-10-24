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
 , ndsCurrentMatrixId_(NDS_MODELVIEW)
{
  // Power control
  REG_POWCNT |= POWER_LCD |POWER_2D_TOP |POWER_2D_BOTTOM | POWER_3D_CORE | POWER_3D_MATRIX;
  // Display control
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
  zA_    = -gl_fpdiv((zFar + zNear), (zFar - zNear));
  zB_    = -gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear));

  MATRIX_MULT4x4 = gl_to_nds(gl_fpdiv((zNear << 1), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(gl_fpdiv((right + left), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(gl_fpdiv((zNear << 1), (top - bottom)));
  MATRIX_MULT4x4 = gl_to_nds(gl_fpdiv((top + bottom), (top - bottom)));
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(zA_);
  MATRIX_MULT4x4 = nds_fpfromi(-1);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(zB_);
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_STORE = ndsCurrentMatrixId_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glLoadIdentity()
{
  MATRIX_IDENTITY = 0;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_PROJECTION: ndsCurrentMatrixId_ = NDS_PROJECTION; break;
    //case GL_???:        ndsCurrentMatrixId_ = NDS_POSITION;   break;
    case GL_MODELVIEW:  ndsCurrentMatrixId_ = NDS_MODELVIEW;  break;
    case GL_TEXTURE:    ndsCurrentMatrixId_ = NDS_TEXTURE;    break;
  };

  MATRIX_CONTROL = ndsCurrentMatrixId_;
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  // Normalize the angle
  angle = angle - gl_fpfromi((gl_fptoi(angle) / 360) * 360);
  if(angle < 0)
    angle += gl_fpfromi(360);

  // Get sin and cos from lookup table
  NDSfixed iSin = nds_fpfromf(sin(gl_fptof(angle) * M_PI / 180.0f));
  NDSfixed iCos = nds_fpfromf(cos(gl_fptof(angle) * M_PI / 180.0f));

  // Convert from gl fixed to nds fixed
  x = gl_to_nds(x);
  y = gl_to_nds(y);
  z = gl_to_nds(z);

  long flags(((z != 0) << 2) | ((y != 0) << 1) | (x != 0));
  switch(flags)
  {
    case 0x00:
    {
      break;
    }
    case 0x01:
    {
      // X Rotation only
      MATRIX_MULT3x3 = nds_fpfromi(1);
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = -iSin;

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iSin;
      MATRIX_MULT3x3 = iCos;
      break;
    }
    case 0x02:
    {
      // Y Rotation only
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iSin;

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(1);
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = -iSin;
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = iCos;
      break;
    }
    case 0x04:
    {
      // Z Rotation only
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = iSin;
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = -iSin;
      MATRIX_MULT3x3 = iCos;
      MATRIX_MULT3x3 = nds_fpfromi(0);

      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(0);
      MATRIX_MULT3x3 = nds_fpfromi(1);
      break;
    }
    default:
    {
      // Mixed Rotation
      NDSfixed iMinCos = nds_fpfromi(1) - iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, x), iMinCos) + iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, y), iMinCos) - nds_fpmul(z, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(x, z), iMinCos) + nds_fpmul(y, iSin);

      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, x), iMinCos) + nds_fpmul(z, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, y), iMinCos) + iCos;
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(y, z), iMinCos) - nds_fpmul(x, iSin);

      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, x), iMinCos) - nds_fpmul(y, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, y), iMinCos) + nds_fpmul(x, iSin);
      MATRIX_MULT3x3 = nds_fpmul(nds_fpmul(z, z), iMinCos) + iCos;
    }
  };
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_SCALE = gl_to_nds(x);
  MATRIX_SCALE = gl_to_nds(y);
  MATRIX_SCALE = gl_to_nds(z);
}

//-----------------------------------------------------------------------------
void
CNDSGLESContext::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_TRANSLATE = gl_to_nds(x);
  MATRIX_TRANSLATE = gl_to_nds(y);
  MATRIX_TRANSLATE = gl_to_nds(z);
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
    GFX_VERTEX16 = ((gl_to_nds(poly.v[i]->v1[1]) << 16) & 0xffff0000) | (gl_to_nds(poly.v[i]->v1[0]) & 0xffff);
    GFX_VERTEX16 = gl_to_nds(poly.v[i]->v1[2]) & 0xffff;
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
