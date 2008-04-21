#include "matrix.h"
#include "string.h"
typedef unsigned int wint_t;
#include <math.h>


#ifdef CONFIG_FPU
static bool      bInitialized_(false);
static GLfloat   fpSin_[360];
static GLfloat   fpCos_[360];
#else
static bool      bInitialized_(false);
static GLfixed   fpSin_[360];
static GLfixed   fpCos_[360];
#endif // CONFIG_FPU


#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void
CMatrixF::frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  GLfloat m[16];

  m[0*4+0] = (2.0f * zNear) / (right - left);
  m[0*4+1] = 0.0f;
  m[0*4+2] = (right + left) / (right - left);
  m[0*4+3] = 0.0f;

  m[1*4+0] = 0.0f;
  m[1*4+1] = (2.0f * zNear) / (top - bottom);
  m[1*4+2] = (top + bottom) / (top - bottom);
  m[1*4+3] = 0.0f;

  m[2*4+0] = 0.0f;
  m[2*4+1] = 0.0f;
  m[2*4+2] = -((zFar + zNear) / (zFar - zNear));
  m[2*4+3] = -((2.0f * zFar * zNear) / (zFar - zNear));

  m[3*4+0] = 0.0f;
  m[3*4+1] = 0.0f;
  m[3*4+2] = -1.0f;
  m[3*4+3] = 0.0f;

  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixF::ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  GLfloat m[16];

  m[0*4+0] = 2.0f / (right - left);
  m[0*4+1] = 0.0f;
  m[0*4+2] = 0.0f;
  m[0*4+3] = -((right + left) / (right - left));

  m[1*4+0] = 0.0f;
  m[1*4+1] = 2.0f / (top - bottom);
  m[1*4+2] = 0.0f;
  m[1*4+3] = -((top + bottom) / (top - bottom));

  m[2*4+0] = 0.0f;
  m[2*4+1] = 0.0f;
  m[2*4+2] = -2.0f / (zFar - zNear);
  m[2*4+3] = -((zFar + zNear) / (zFar - zNear));

  m[3*4+0] = 0.0f;
  m[3*4+1] = 0.0f;
  m[3*4+2] = 0.0f;
  m[3*4+3] = 1.0f;

  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixF::translate(GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat m[16];
  matrixf_identity(m);
  m[0*4+3] = x;
  m[1*4+3] = y;
  m[2*4+3] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixF::scale(GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat m[16];
  matrixf_identity(m);
  m[0*4+0] = x;
  m[1*4+1] = y;
  m[2*4+2] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixF::rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i(0); i < 360; i++)
    {
      fpSin_[i] = sin(static_cast<float>(i) * M_PI / 180.0f);
      fpCos_[i] = cos(static_cast<float>(i) * M_PI / 180.0f);
    }
  }

  // Normalize the angle
  angle = angle - (((int)angle / 360) * 360);
  if(angle < 0.0f)
    angle += 360.0f;
  // Get sin and cos from lookup table
  GLfloat iSin = fpSin_[(int)angle % 360];
  GLfloat iCos = fpCos_[(int)angle % 360];

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
      GLfloat m[16];
      matrixf_identity(m);
      m[1*4+1] = iCos;
      m[1*4+2] = -iSin;
      m[2*4+1] = iSin;
      m[2*4+2] = iCos;
      *this *= m;
      break;
    }
    case 0x02:
    {
      // Y Rotation only
      GLfloat m[16];
      matrixf_identity(m);
      m[0*4+0] = iCos;
      m[0*4+2] = iSin;
      m[2*4+0] = -iSin;
      m[2*4+2] = iCos;
      *this *= m;
      break;
    }
    case 0x04:
    {
      // Z Rotation only
      GLfloat m[16];
      matrixf_identity(m);
      m[0*4+0] = iCos;
      m[0*4+1] = iSin;
      m[1*4+0] = -iSin;
      m[1*4+1] = iCos;
      *this *= m;
      break;
    }
    default:
    {
      // Mixed Rotation
      GLfloat iMinCos = 1.0f - iCos;
      GLfloat m[16];
      m[0*4+0] = x * x * iMinCos + iCos;
      m[0*4+1] = x * y * iMinCos - z * iSin;
      m[0*4+2] = x * z * iMinCos + y * iSin;
      m[0*4+3] = 0.0f;
      m[1*4+0] = y * x * iMinCos + z * iSin;
      m[1*4+1] = y * y * iMinCos + iCos;
      m[1*4+2] = y * z * iMinCos - x * iSin;
      m[1*4+3] = 0.0f;
      m[2*4+0] = z * x * iMinCos - y * iSin;
      m[2*4+1] = z * y * iMinCos + x * iSin;
      m[2*4+2] = z * z * iMinCos + iCos;
      m[2*4+3] = 0.0f;
      m[3*4+0] = 0.0f;
      m[3*4+1] = 0.0f;
      m[3*4+2] = 0.0f;
      m[3*4+3] = 1.0f;
      *this *= m;
    }
  };
}

//---------------------------------------------------------------------------
// 64 x operator*
// 48 x operator+
CMatrixF &
CMatrixF::operator*=(const CMatrixF & m)
{
  GLfloat mtemp[16];

  mtemp[0*4+0] = m00 * m.m00 + m01 * m.m10 + m02 * m.m20 + m03 * m.m30;
  mtemp[0*4+1] = m00 * m.m01 + m01 * m.m11 + m02 * m.m21 + m03 * m.m31;
  mtemp[0*4+2] = m00 * m.m02 + m01 * m.m12 + m02 * m.m22 + m03 * m.m32;
  mtemp[0*4+3] = m00 * m.m03 + m01 * m.m13 + m02 * m.m23 + m03 * m.m33;

  mtemp[1*4+0] = m10 * m.m00 + m11 * m.m10 + m12 * m.m20 + m13 * m.m30;
  mtemp[1*4+1] = m10 * m.m01 + m11 * m.m11 + m12 * m.m21 + m13 * m.m31;
  mtemp[1*4+2] = m10 * m.m02 + m11 * m.m12 + m12 * m.m22 + m13 * m.m32;
  mtemp[1*4+3] = m10 * m.m03 + m11 * m.m13 + m12 * m.m23 + m13 * m.m33;

  mtemp[2*4+0] = m20 * m.m00 + m21 * m.m10 + m22 * m.m20 + m23 * m.m30;
  mtemp[2*4+1] = m20 * m.m01 + m21 * m.m11 + m22 * m.m21 + m23 * m.m31;
  mtemp[2*4+2] = m20 * m.m02 + m21 * m.m12 + m22 * m.m22 + m23 * m.m32;
  mtemp[2*4+3] = m20 * m.m03 + m21 * m.m13 + m22 * m.m23 + m23 * m.m33;

  mtemp[3*4+0] = m30 * m.m00 + m31 * m.m10 + m32 * m.m20 + m33 * m.m30;
  mtemp[3*4+1] = m30 * m.m01 + m31 * m.m11 + m32 * m.m21 + m33 * m.m31;
  mtemp[3*4+2] = m30 * m.m02 + m31 * m.m12 + m32 * m.m22 + m33 * m.m32;
  mtemp[3*4+3] = m30 * m.m03 + m31 * m.m13 + m32 * m.m23 + m33 * m.m33;

  matrixf_copy(matrix, mtemp);

  return(*this);
}

//---------------------------------------------------------------------------
CMatrixF &
CMatrixF::operator*=(const GLfloat * m)
{
  GLfloat mtemp[16];

  mtemp[0*4+0] = m00 * m[0*4+0] + m01 * m[1*4+0] + m02 * m[2*4+0] + m03 * m[3*4+0];
  mtemp[0*4+1] = m00 * m[0*4+1] + m01 * m[1*4+1] + m02 * m[2*4+1] + m03 * m[3*4+1];
  mtemp[0*4+2] = m00 * m[0*4+2] + m01 * m[1*4+2] + m02 * m[2*4+2] + m03 * m[3*4+2];
  mtemp[0*4+3] = m00 * m[0*4+3] + m01 * m[1*4+3] + m02 * m[2*4+3] + m03 * m[3*4+3];

  mtemp[1*4+0] = m10 * m[0*4+0] + m11 * m[1*4+0] + m12 * m[2*4+0] + m13 * m[3*4+0];
  mtemp[1*4+1] = m10 * m[0*4+1] + m11 * m[1*4+1] + m12 * m[2*4+1] + m13 * m[3*4+1];
  mtemp[1*4+2] = m10 * m[0*4+2] + m11 * m[1*4+2] + m12 * m[2*4+2] + m13 * m[3*4+2];
  mtemp[1*4+3] = m10 * m[0*4+3] + m11 * m[1*4+3] + m12 * m[2*4+3] + m13 * m[3*4+3];

  mtemp[2*4+0] = m20 * m[0*4+0] + m21 * m[1*4+0] + m22 * m[2*4+0] + m23 * m[3*4+0];
  mtemp[2*4+1] = m20 * m[0*4+1] + m21 * m[1*4+1] + m22 * m[2*4+1] + m23 * m[3*4+1];
  mtemp[2*4+2] = m20 * m[0*4+2] + m21 * m[1*4+2] + m22 * m[2*4+2] + m23 * m[3*4+2];
  mtemp[2*4+3] = m20 * m[0*4+3] + m21 * m[1*4+3] + m22 * m[2*4+3] + m23 * m[3*4+3];

  mtemp[3*4+0] = m30 * m[0*4+0] + m31 * m[1*4+0] + m32 * m[2*4+0] + m33 * m[3*4+0];
  mtemp[3*4+1] = m30 * m[0*4+1] + m31 * m[1*4+1] + m32 * m[2*4+1] + m33 * m[3*4+1];
  mtemp[3*4+2] = m30 * m[0*4+2] + m31 * m[1*4+2] + m32 * m[2*4+2] + m33 * m[3*4+2];
  mtemp[3*4+3] = m30 * m[0*4+3] + m31 * m[1*4+3] + m32 * m[2*4+3] + m33 * m[3*4+3];

  matrixf_copy(matrix, mtemp);

  return(*this);
}
#else // CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void
CMatrixFx::frustum(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  GLfixed m[16];

  m[0*4+0] = gl_fpdiv((zNear << 1), (right - left));
  m[0*4+1] = gl_fpfromi(0);
  m[0*4+2] = gl_fpdiv((right + left), (right - left));
  m[0*4+3] = gl_fpfromi(0);

  m[1*4+0] = gl_fpfromi(0);
  m[1*4+1] = gl_fpdiv((zNear << 1), (top - bottom));
  m[1*4+2] = gl_fpdiv((top + bottom), (top - bottom));
  m[1*4+3] = gl_fpfromi(0);

  m[2*4+0] = gl_fpfromi(0);
  m[2*4+1] = gl_fpfromi(0);
  m[2*4+2] = -gl_fpdiv((zFar + zNear), (zFar - zNear));
  m[2*4+3] = -gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear));

  m[3*4+0] = gl_fpfromi(0);
  m[3*4+1] = gl_fpfromi(0);
  m[3*4+2] = gl_fpfromi(-1);
  m[3*4+3] = gl_fpfromi(0);

  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::ortho(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  GLfixed m[16];

  m[0*4+0] = gl_fpdiv(gl_fpfromi(2), (right - left));
  m[0*4+1] = gl_fpfromi(0);
  m[0*4+2] = gl_fpfromi(0);
  m[0*4+3] = -gl_fpdiv((right + left), (right - left));

  m[1*4+0] = gl_fpfromi(0);
  m[1*4+1] = gl_fpdiv(gl_fpfromi(2), (top - bottom));
  m[1*4+2] = gl_fpfromi(0);
  m[1*4+3] = -gl_fpdiv((top + bottom), (top - bottom));

  m[2*4+0] = gl_fpfromi(0);
  m[2*4+1] = gl_fpfromi(0);
  m[2*4+2] = gl_fpdiv(gl_fpfromi(-2), (zFar - zNear));
  m[2*4+3] = -gl_fpdiv((zFar + zNear), (zFar - zNear));

  m[3*4+0] = gl_fpfromi(0);
  m[3*4+1] = gl_fpfromi(0);
  m[3*4+2] = gl_fpfromi(0);
  m[3*4+3] = gl_fpfromi(1);

  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::translate(GLfixed x, GLfixed y, GLfixed z)
{
  GLfixed m[16];
  matrixfx_identity(m);
  m[0*4+3] = x;
  m[1*4+3] = y;
  m[2*4+3] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::scale(GLfixed x, GLfixed y, GLfixed z)
{
  GLfixed m[16];
  matrixfx_identity(m);
  m[0*4+0] = x;
  m[1*4+1] = y;
  m[2*4+2] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::rotate(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i(0); i < 360; i++)
    {
      fpSin_[i] = gl_fpfromf(sin(static_cast<float>(i) * M_PI / 180.0f));
      fpCos_[i] = gl_fpfromf(cos(static_cast<float>(i) * M_PI / 180.0f));
    }
  }

  // Normalize the angle
  angle = angle - gl_fpfromi((gl_fptoi(angle) / 360) * 360);
  if(angle < 0)
    angle += gl_fpfromi(360);
  // Get sin and cos from lookup table
  GLfixed iSin = fpSin_[gl_fptoi(angle) % 360];
  GLfixed iCos = fpCos_[gl_fptoi(angle) % 360];

  // Convert from gl fixed to m fixed
  x = x;
  y = y;
  z = z;

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
      GLfixed m[16];
      matrixfx_identity(m);
      m[1*4+1] = iCos;
      m[1*4+2] = -iSin;
      m[2*4+1] = iSin;
      m[2*4+2] = iCos;
      *this *= m;
      break;
    }
    case 0x02:
    {
      // Y Rotation only
      GLfixed m[16];
      matrixfx_identity(m);
      m[0*4+0] = iCos;
      m[0*4+2] = iSin;
      m[2*4+0] = -iSin;
      m[2*4+2] = iCos;
      *this *= m;
      break;
    }
    case 0x04:
    {
      // Z Rotation only
      GLfixed m[16];
      matrixfx_identity(m);
      m[0*4+0] = iCos;
      m[0*4+1] = iSin;
      m[1*4+0] = -iSin;
      m[1*4+1] = iCos;
      *this *= m;
      break;
    }
    default:
    {
      // Mixed Rotation
      GLfixed iMinCos = gl_fpfromi(1) - iCos;
      GLfixed m[16];
      m[0*4+0] = gl_fpmul(gl_fpmul(x, x), iMinCos) + iCos;
      m[0*4+1] = gl_fpmul(gl_fpmul(x, y), iMinCos) - gl_fpmul(z, iSin);
      m[0*4+2] = gl_fpmul(gl_fpmul(x, z), iMinCos) + gl_fpmul(y, iSin);
      m[0*4+3] = gl_fpfromi(0);
      m[1*4+0] = gl_fpmul(gl_fpmul(y, x), iMinCos) + gl_fpmul(z, iSin);
      m[1*4+1] = gl_fpmul(gl_fpmul(y, y), iMinCos) + iCos;
      m[1*4+2] = gl_fpmul(gl_fpmul(y, z), iMinCos) - gl_fpmul(x, iSin);
      m[1*4+3] = gl_fpfromi(0);
      m[2*4+0] = gl_fpmul(gl_fpmul(z, x), iMinCos) - gl_fpmul(y, iSin);
      m[2*4+1] = gl_fpmul(gl_fpmul(z, y), iMinCos) + gl_fpmul(x, iSin);
      m[2*4+2] = gl_fpmul(gl_fpmul(z, z), iMinCos) + iCos;
      m[2*4+3] = gl_fpfromi(0);
      m[3*4+0] = gl_fpfromi(0);
      m[3*4+1] = gl_fpfromi(0);
      m[3*4+2] = gl_fpfromi(0);
      m[3*4+3] = gl_fpfromi(1);
      *this *= m;
    }
  };
}

//---------------------------------------------------------------------------
// 64 x operator*
// 48 x operator+
CMatrixFx &
CMatrixFx::operator*=(const CMatrixFx & m)
{
  GLfixed mtemp[16];

  mtemp[0*4+0] = gl_fpmul(m00, m.m00) + gl_fpmul(m01, m.m10) + gl_fpmul(m02, m.m20) + gl_fpmul(m03, m.m30);
  mtemp[0*4+1] = gl_fpmul(m00, m.m01) + gl_fpmul(m01, m.m11) + gl_fpmul(m02, m.m21) + gl_fpmul(m03, m.m31);
  mtemp[0*4+2] = gl_fpmul(m00, m.m02) + gl_fpmul(m01, m.m12) + gl_fpmul(m02, m.m22) + gl_fpmul(m03, m.m32);
  mtemp[0*4+3] = gl_fpmul(m00, m.m03) + gl_fpmul(m01, m.m13) + gl_fpmul(m02, m.m23) + gl_fpmul(m03, m.m33);

  mtemp[1*4+0] = gl_fpmul(m10, m.m00) + gl_fpmul(m11, m.m10) + gl_fpmul(m12, m.m20) + gl_fpmul(m13, m.m30);
  mtemp[1*4+1] = gl_fpmul(m10, m.m01) + gl_fpmul(m11, m.m11) + gl_fpmul(m12, m.m21) + gl_fpmul(m13, m.m31);
  mtemp[1*4+2] = gl_fpmul(m10, m.m02) + gl_fpmul(m11, m.m12) + gl_fpmul(m12, m.m22) + gl_fpmul(m13, m.m32);
  mtemp[1*4+3] = gl_fpmul(m10, m.m03) + gl_fpmul(m11, m.m13) + gl_fpmul(m12, m.m23) + gl_fpmul(m13, m.m33);

  mtemp[2*4+0] = gl_fpmul(m20, m.m00) + gl_fpmul(m21, m.m10) + gl_fpmul(m22, m.m20) + gl_fpmul(m23, m.m30);
  mtemp[2*4+1] = gl_fpmul(m20, m.m01) + gl_fpmul(m21, m.m11) + gl_fpmul(m22, m.m21) + gl_fpmul(m23, m.m31);
  mtemp[2*4+2] = gl_fpmul(m20, m.m02) + gl_fpmul(m21, m.m12) + gl_fpmul(m22, m.m22) + gl_fpmul(m23, m.m32);
  mtemp[2*4+3] = gl_fpmul(m20, m.m03) + gl_fpmul(m21, m.m13) + gl_fpmul(m22, m.m23) + gl_fpmul(m23, m.m33);

  mtemp[3*4+0] = gl_fpmul(m30, m.m00) + gl_fpmul(m31, m.m10) + gl_fpmul(m32, m.m20) + gl_fpmul(m33, m.m30);
  mtemp[3*4+1] = gl_fpmul(m30, m.m01) + gl_fpmul(m31, m.m11) + gl_fpmul(m32, m.m21) + gl_fpmul(m33, m.m31);
  mtemp[3*4+2] = gl_fpmul(m30, m.m02) + gl_fpmul(m31, m.m12) + gl_fpmul(m32, m.m22) + gl_fpmul(m33, m.m32);
  mtemp[3*4+3] = gl_fpmul(m30, m.m03) + gl_fpmul(m31, m.m13) + gl_fpmul(m32, m.m23) + gl_fpmul(m33, m.m33);

  matrixfx_copy(matrix, mtemp);

  return (*this);
}

//---------------------------------------------------------------------------
CMatrixFx &
CMatrixFx::operator*=(const GLfixed * m)
{
  GLfixed mtemp[16];

  mtemp[0*4+0] = gl_fpmul(m00, m[0*4+0]) + gl_fpmul(m01, m[1*4+0]) + gl_fpmul(m02, m[2*4+0]) + gl_fpmul(m03, m[3*4+0]);
  mtemp[0*4+1] = gl_fpmul(m00, m[0*4+1]) + gl_fpmul(m01, m[1*4+1]) + gl_fpmul(m02, m[2*4+1]) + gl_fpmul(m03, m[3*4+1]);
  mtemp[0*4+2] = gl_fpmul(m00, m[0*4+2]) + gl_fpmul(m01, m[1*4+2]) + gl_fpmul(m02, m[2*4+2]) + gl_fpmul(m03, m[3*4+2]);
  mtemp[0*4+3] = gl_fpmul(m00, m[0*4+3]) + gl_fpmul(m01, m[1*4+3]) + gl_fpmul(m02, m[2*4+3]) + gl_fpmul(m03, m[3*4+3]);

  mtemp[1*4+0] = gl_fpmul(m10, m[0*4+0]) + gl_fpmul(m11, m[1*4+0]) + gl_fpmul(m12, m[2*4+0]) + gl_fpmul(m13, m[3*4+0]);
  mtemp[1*4+1] = gl_fpmul(m10, m[0*4+1]) + gl_fpmul(m11, m[1*4+1]) + gl_fpmul(m12, m[2*4+1]) + gl_fpmul(m13, m[3*4+1]);
  mtemp[1*4+2] = gl_fpmul(m10, m[0*4+2]) + gl_fpmul(m11, m[1*4+2]) + gl_fpmul(m12, m[2*4+2]) + gl_fpmul(m13, m[3*4+2]);
  mtemp[1*4+3] = gl_fpmul(m10, m[0*4+3]) + gl_fpmul(m11, m[1*4+3]) + gl_fpmul(m12, m[2*4+3]) + gl_fpmul(m13, m[3*4+3]);

  mtemp[2*4+0] = gl_fpmul(m20, m[0*4+0]) + gl_fpmul(m21, m[1*4+0]) + gl_fpmul(m22, m[2*4+0]) + gl_fpmul(m23, m[3*4+0]);
  mtemp[2*4+1] = gl_fpmul(m20, m[0*4+1]) + gl_fpmul(m21, m[1*4+1]) + gl_fpmul(m22, m[2*4+1]) + gl_fpmul(m23, m[3*4+1]);
  mtemp[2*4+2] = gl_fpmul(m20, m[0*4+2]) + gl_fpmul(m21, m[1*4+2]) + gl_fpmul(m22, m[2*4+2]) + gl_fpmul(m23, m[3*4+2]);
  mtemp[2*4+3] = gl_fpmul(m20, m[0*4+3]) + gl_fpmul(m21, m[1*4+3]) + gl_fpmul(m22, m[2*4+3]) + gl_fpmul(m23, m[3*4+3]);

  mtemp[3*4+0] = gl_fpmul(m30, m[0*4+0]) + gl_fpmul(m31, m[1*4+0]) + gl_fpmul(m32, m[2*4+0]) + gl_fpmul(m33, m[3*4+0]);
  mtemp[3*4+1] = gl_fpmul(m30, m[0*4+1]) + gl_fpmul(m31, m[1*4+1]) + gl_fpmul(m32, m[2*4+1]) + gl_fpmul(m33, m[3*4+1]);
  mtemp[3*4+2] = gl_fpmul(m30, m[0*4+2]) + gl_fpmul(m31, m[1*4+2]) + gl_fpmul(m32, m[2*4+2]) + gl_fpmul(m33, m[3*4+2]);
  mtemp[3*4+3] = gl_fpmul(m30, m[0*4+3]) + gl_fpmul(m31, m[1*4+3]) + gl_fpmul(m32, m[2*4+3]) + gl_fpmul(m33, m[3*4+3]);

  matrixfx_copy(matrix, mtemp);

  return (*this);
}
#endif // CONFIG_FPU

#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixF::CAGLESMatrixF()
 : matrixMode_(GL_MODELVIEW)
 , pCurrentMatrix_(&matrixModelView)
{
}

//---------------------------------------------------------------------------
CAGLESMatrixF::~CAGLESMatrixF()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  pCurrentMatrix_->frustum(left, right, bottom, top, zNear, zFar);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glLoadMatrixf(const GLfloat *m)
{
  *pCurrentMatrix_ = m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glMultMatrixf(const GLfloat *m)
{
  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  pCurrentMatrix_->ortho(left, right, bottom, top, zNear, zFar);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);
  if(matrixMode_ == GL_MODELVIEW)
    matrixNormal.rotate(angle, x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->scale(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->translate(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glLoadIdentity(void)
{
  pCurrentMatrix_->loadIdentity();
  if(matrixMode_ == GL_MODELVIEW)
    matrixNormal.loadIdentity();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_MODELVIEW:  pCurrentMatrix_ = &matrixModelView;  break;
    case GL_PROJECTION: pCurrentMatrix_ = &matrixProjection; break;
    case GL_TEXTURE:    pCurrentMatrix_ = &matrixTexture;    break;
  };
}
#else // CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixFx::CAGLESMatrixFx()
 : matrixMode_(GL_MODELVIEW)
 , pCurrentMatrix_(&matrixModelView)
{
}

//---------------------------------------------------------------------------
CAGLESMatrixFx::~CAGLESMatrixFx()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  pCurrentMatrix_->frustum(left, right, bottom, top, zNear, zFar);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glLoadMatrixx(const GLfixed *m)
{
  *pCurrentMatrix_ = m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glMultMatrixx(const GLfixed *m)
{
  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  pCurrentMatrix_->ortho(left, right, bottom, top, zNear, zFar);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);
  if(matrixMode_ == GL_MODELVIEW)
    matrixNormal.rotate(angle, x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->scale(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->translate(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glLoadIdentity(void)
{
  pCurrentMatrix_->loadIdentity();
  if(matrixMode_ == GL_MODELVIEW)
    matrixNormal.loadIdentity();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_MODELVIEW:  pCurrentMatrix_ = &matrixModelView;  break;
    case GL_PROJECTION: pCurrentMatrix_ = &matrixProjection; break;
    case GL_TEXTURE:    pCurrentMatrix_ = &matrixTexture;    break;
  };
}
#endif // CONFIG_FPU
