#include "matrixNDS.h"
#include "asm/arch/registers.h"
#include "asm/arch/macros.h"
#include "string.h"
typedef unsigned int wint_t;
#include <math.h>




bool      CAGLESMatrixNDSFx::bInitialized_(false);
NDSfixed  CAGLESMatrixNDSFx::fpSin_[360];
NDSfixed  CAGLESMatrixNDSFx::fpCos_[360];


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixNDSFx::CAGLESMatrixNDSFx()
 : matrixMode_(GL_MODELVIEW)
 , ndsCurrentMatrixId_(NDS_MODELVIEW)
{
  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i(0); i < 360; i++)
    {
      fpSin_[i] = nds_fpfromf(sin(static_cast<float>(i) * M_PI / 180.0f));
      fpCos_[i] = nds_fpfromf(cos(static_cast<float>(i) * M_PI / 180.0f));
    }
  }

  zNear_ = gl_fpfromf(  0.1f);
  zFar_  = gl_fpfromf(100.0f);
  zA_    = -gl_fpdiv((zFar_ + zNear_), (zFar_ - zNear_));
  zB_    = -gl_fpdiv((gl_fpmul(zFar_, zNear_) << 1), (zFar_ - zNear_));
}

//---------------------------------------------------------------------------
CAGLESMatrixNDSFx::~CAGLESMatrixNDSFx()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixNDSFx::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
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

//  MATRIX_STORE = ndsCurrentMatrixId_;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixNDSFx::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  zNear_ = zNear;
  zFar_  = zFar;
  zA_    = -gl_fpdiv((zFar + zNear), (zFar - zNear));
  zB_    = -gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear));

  MATRIX_MULT4x4 = gl_to_nds(gl_fpdiv(gl_fpfromi(2), (right - left)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(-gl_fpdiv((right + left), (right - left)));

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(gl_fpdiv(gl_fpfromi(2), (top - bottom)));
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(-gl_fpdiv((top + bottom), (top - bottom)));

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(zA_);
  MATRIX_MULT4x4 = nds_fpfromi(0);

  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = nds_fpfromi(0);
  MATRIX_MULT4x4 = gl_to_nds(zB_);
  MATRIX_MULT4x4 = nds_fpfromi(1);

//  MATRIX_STORE = ndsCurrentMatrixId_;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixNDSFx::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  // Normalize the angle
  angle = angle - gl_fpfromi((gl_fptoi(angle) / 360) * 360);
  if(angle < 0)
    angle += gl_fpfromi(360);

  // Get sin and cos from lookup table
  NDSfixed iSin = fpSin_[gl_fptoi(angle) % 360];
  NDSfixed iCos = fpCos_[gl_fptoi(angle) % 360];

  // Convert from gl fixed to nds fixed
  iSin = -iSin; // Why???
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

//---------------------------------------------------------------------------
void
CAGLESMatrixNDSFx::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_SCALE = gl_to_nds(x);
  MATRIX_SCALE = gl_to_nds(y);
  MATRIX_SCALE = gl_to_nds(z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixNDSFx::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  MATRIX_TRANSLATE = gl_to_nds(x);
  MATRIX_TRANSLATE = gl_to_nds(y);
  MATRIX_TRANSLATE = gl_to_nds(z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixNDSFx::glLoadIdentity(void)
{
  MATRIX_IDENTITY = 0;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixNDSFx::glMatrixMode(GLenum mode)
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
