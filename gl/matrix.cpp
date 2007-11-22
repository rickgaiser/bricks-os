#include "matrix.h"
#include "string.h"
typedef unsigned int wint_t;
#include <math.h>


#ifdef CONFIG_FPU
bool      CMatrixF::bInitialized_(false);
GLfloat   CMatrixF::fpSin_[360];
GLfloat   CMatrixF::fpCos_[360];
#else
bool      CMatrixFx::bInitialized_(false);
Mfixed    CMatrixFx::fpSin_[360];
Mfixed    CMatrixFx::fpCos_[360];
#endif // CONFIG_FPU


#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CMatrixF::CMatrixF()
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
}

//---------------------------------------------------------------------------
CMatrixF::~CMatrixF()
{
}

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

  mtemp[0*4+0] = matrix[0*4+0] * m.matrix[0*4+0] + matrix[0*4+1] * m.matrix[1*4+0] + matrix[0*4+2] * m.matrix[2*4+0] + matrix[0*4+3] * m.matrix[3*4+0];
  mtemp[0*4+1] = matrix[0*4+0] * m.matrix[0*4+1] + matrix[0*4+1] * m.matrix[1*4+1] + matrix[0*4+2] * m.matrix[2*4+1] + matrix[0*4+3] * m.matrix[3*4+1];
  mtemp[0*4+2] = matrix[0*4+0] * m.matrix[0*4+2] + matrix[0*4+1] * m.matrix[1*4+2] + matrix[0*4+2] * m.matrix[2*4+2] + matrix[0*4+3] * m.matrix[3*4+2];
  mtemp[0*4+3] = matrix[0*4+0] * m.matrix[0*4+3] + matrix[0*4+1] * m.matrix[1*4+3] + matrix[0*4+2] * m.matrix[2*4+3] + matrix[0*4+3] * m.matrix[3*4+3];

  mtemp[1*4+0] = matrix[1*4+0] * m.matrix[0*4+0] + matrix[1*4+1] * m.matrix[1*4+0] + matrix[1*4+2] * m.matrix[2*4+0] + matrix[1*4+3] * m.matrix[3*4+0];
  mtemp[1*4+1] = matrix[1*4+0] * m.matrix[0*4+1] + matrix[1*4+1] * m.matrix[1*4+1] + matrix[1*4+2] * m.matrix[2*4+1] + matrix[1*4+3] * m.matrix[3*4+1];
  mtemp[1*4+2] = matrix[1*4+0] * m.matrix[0*4+2] + matrix[1*4+1] * m.matrix[1*4+2] + matrix[1*4+2] * m.matrix[2*4+2] + matrix[1*4+3] * m.matrix[3*4+2];
  mtemp[1*4+3] = matrix[1*4+0] * m.matrix[0*4+3] + matrix[1*4+1] * m.matrix[1*4+3] + matrix[1*4+2] * m.matrix[2*4+3] + matrix[1*4+3] * m.matrix[3*4+3];

  mtemp[2*4+0] = matrix[2*4+0] * m.matrix[0*4+0] + matrix[2*4+1] * m.matrix[1*4+0] + matrix[2*4+2] * m.matrix[2*4+0] + matrix[2*4+3] * m.matrix[3*4+0];
  mtemp[2*4+1] = matrix[2*4+0] * m.matrix[0*4+1] + matrix[2*4+1] * m.matrix[1*4+1] + matrix[2*4+2] * m.matrix[2*4+1] + matrix[2*4+3] * m.matrix[3*4+1];
  mtemp[2*4+2] = matrix[2*4+0] * m.matrix[0*4+2] + matrix[2*4+1] * m.matrix[1*4+2] + matrix[2*4+2] * m.matrix[2*4+2] + matrix[2*4+3] * m.matrix[3*4+2];
  mtemp[2*4+3] = matrix[2*4+0] * m.matrix[0*4+3] + matrix[2*4+1] * m.matrix[1*4+3] + matrix[2*4+2] * m.matrix[2*4+3] + matrix[2*4+3] * m.matrix[3*4+3];

  mtemp[3*4+0] = matrix[3*4+0] * m.matrix[0*4+0] + matrix[3*4+1] * m.matrix[1*4+0] + matrix[3*4+2] * m.matrix[2*4+0] + matrix[3*4+3] * m.matrix[3*4+0];
  mtemp[3*4+1] = matrix[3*4+0] * m.matrix[0*4+1] + matrix[3*4+1] * m.matrix[1*4+1] + matrix[3*4+2] * m.matrix[2*4+1] + matrix[3*4+3] * m.matrix[3*4+1];
  mtemp[3*4+2] = matrix[3*4+0] * m.matrix[0*4+2] + matrix[3*4+1] * m.matrix[1*4+2] + matrix[3*4+2] * m.matrix[2*4+2] + matrix[3*4+3] * m.matrix[3*4+2];
  mtemp[3*4+3] = matrix[3*4+0] * m.matrix[0*4+3] + matrix[3*4+1] * m.matrix[1*4+3] + matrix[3*4+2] * m.matrix[2*4+3] + matrix[3*4+3] * m.matrix[3*4+3];

  matrixf_copy(matrix, mtemp);

  return(*this);
}

//---------------------------------------------------------------------------
CMatrixF &
CMatrixF::operator*=(const GLfloat * m)
{
  GLfloat mtemp[16];

  mtemp[0*4+0] = matrix[0*4+0] * m[0*4+0] + matrix[0*4+1] * m[1*4+0] + matrix[0*4+2] * m[2*4+0] + matrix[0*4+3] * m[3*4+0];
  mtemp[0*4+1] = matrix[0*4+0] * m[0*4+1] + matrix[0*4+1] * m[1*4+1] + matrix[0*4+2] * m[2*4+1] + matrix[0*4+3] * m[3*4+1];
  mtemp[0*4+2] = matrix[0*4+0] * m[0*4+2] + matrix[0*4+1] * m[1*4+2] + matrix[0*4+2] * m[2*4+2] + matrix[0*4+3] * m[3*4+2];
  mtemp[0*4+3] = matrix[0*4+0] * m[0*4+3] + matrix[0*4+1] * m[1*4+3] + matrix[0*4+2] * m[2*4+3] + matrix[0*4+3] * m[3*4+3];

  mtemp[1*4+0] = matrix[1*4+0] * m[0*4+0] + matrix[1*4+1] * m[1*4+0] + matrix[1*4+2] * m[2*4+0] + matrix[1*4+3] * m[3*4+0];
  mtemp[1*4+1] = matrix[1*4+0] * m[0*4+1] + matrix[1*4+1] * m[1*4+1] + matrix[1*4+2] * m[2*4+1] + matrix[1*4+3] * m[3*4+1];
  mtemp[1*4+2] = matrix[1*4+0] * m[0*4+2] + matrix[1*4+1] * m[1*4+2] + matrix[1*4+2] * m[2*4+2] + matrix[1*4+3] * m[3*4+2];
  mtemp[1*4+3] = matrix[1*4+0] * m[0*4+3] + matrix[1*4+1] * m[1*4+3] + matrix[1*4+2] * m[2*4+3] + matrix[1*4+3] * m[3*4+3];

  mtemp[2*4+0] = matrix[2*4+0] * m[0*4+0] + matrix[2*4+1] * m[1*4+0] + matrix[2*4+2] * m[2*4+0] + matrix[2*4+3] * m[3*4+0];
  mtemp[2*4+1] = matrix[2*4+0] * m[0*4+1] + matrix[2*4+1] * m[1*4+1] + matrix[2*4+2] * m[2*4+1] + matrix[2*4+3] * m[3*4+1];
  mtemp[2*4+2] = matrix[2*4+0] * m[0*4+2] + matrix[2*4+1] * m[1*4+2] + matrix[2*4+2] * m[2*4+2] + matrix[2*4+3] * m[3*4+2];
  mtemp[2*4+3] = matrix[2*4+0] * m[0*4+3] + matrix[2*4+1] * m[1*4+3] + matrix[2*4+2] * m[2*4+3] + matrix[2*4+3] * m[3*4+3];

  mtemp[3*4+0] = matrix[3*4+0] * m[0*4+0] + matrix[3*4+1] * m[1*4+0] + matrix[3*4+2] * m[2*4+0] + matrix[3*4+3] * m[3*4+0];
  mtemp[3*4+1] = matrix[3*4+0] * m[0*4+1] + matrix[3*4+1] * m[1*4+1] + matrix[3*4+2] * m[2*4+1] + matrix[3*4+3] * m[3*4+1];
  mtemp[3*4+2] = matrix[3*4+0] * m[0*4+2] + matrix[3*4+1] * m[1*4+2] + matrix[3*4+2] * m[2*4+2] + matrix[3*4+3] * m[3*4+2];
  mtemp[3*4+3] = matrix[3*4+0] * m[0*4+3] + matrix[3*4+1] * m[1*4+3] + matrix[3*4+2] * m[2*4+3] + matrix[3*4+3] * m[3*4+3];

  matrixf_copy(matrix, mtemp);

  return(*this);
}
#else
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CMatrixFx::CMatrixFx()
{
  if(bInitialized_ == false)
  {
    bInitialized_ = true;
    for(int i(0); i < 360; i++)
    {
      fpSin_[i] = m_fpfromf(sin(static_cast<float>(i) * M_PI / 180.0f));
      fpCos_[i] = m_fpfromf(cos(static_cast<float>(i) * M_PI / 180.0f));
    }
  }
}

//---------------------------------------------------------------------------
CMatrixFx::~CMatrixFx()
{
}

//---------------------------------------------------------------------------
void
CMatrixFx::frustum(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  Mfixed m[16];

  m[0*4+0] = gl_to_m(gl_fpdiv((zNear << 1), (right - left)));
  m[0*4+1] = m_fpfromi(0);
  m[0*4+2] = gl_to_m(gl_fpdiv((right + left), (right - left)));
  m[0*4+3] = m_fpfromi(0);

  m[1*4+0] = m_fpfromi(0);
  m[1*4+1] = gl_to_m(gl_fpdiv((zNear << 1), (top - bottom)));
  m[1*4+2] = gl_to_m(gl_fpdiv((top + bottom), (top - bottom)));
  m[1*4+3] = m_fpfromi(0);

  m[2*4+0] = m_fpfromi(0);
  m[2*4+1] = m_fpfromi(0);
  m[2*4+2] = gl_to_m(-gl_fpdiv((zFar + zNear), (zFar - zNear)));
  m[2*4+3] = gl_to_m(-gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear)));

  m[3*4+0] = m_fpfromi(0);
  m[3*4+1] = m_fpfromi(0);
  m[3*4+2] = m_fpfromi(-1);
  m[3*4+3] = m_fpfromi(0);

  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::ortho(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  Mfixed m[16];

  m[0*4+0] = gl_to_m(gl_fpdiv(gl_fpfromi(2), (right - left)));
  m[0*4+1] = m_fpfromi(0);
  m[0*4+2] = m_fpfromi(0);
  m[0*4+3] = gl_to_m(-gl_fpdiv((right + left), (right - left)));

  m[1*4+0] = m_fpfromi(0);
  m[1*4+1] = gl_to_m(gl_fpdiv(gl_fpfromi(2), (top - bottom)));
  m[1*4+2] = m_fpfromi(0);
  m[1*4+3] = gl_to_m(-gl_fpdiv((top + bottom), (top - bottom)));

  m[2*4+0] = m_fpfromi(0);
  m[2*4+1] = m_fpfromi(0);
  m[2*4+2] = gl_to_m(gl_fpdiv(gl_fpfromi(-2), (zFar - zNear)));
  m[2*4+3] = gl_to_m(-gl_fpdiv((zFar + zNear), (zFar - zNear)));

  m[3*4+0] = m_fpfromi(0);
  m[3*4+1] = m_fpfromi(0);
  m[3*4+2] = m_fpfromi(0);
  m[3*4+3] = m_fpfromi(1);

  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::translate(GLfixed x, GLfixed y, GLfixed z)
{
  Mfixed m[16];
  matrixfx_identity(m);
  m[0*4+3] = gl_to_m(x);
  m[1*4+3] = gl_to_m(y);
  m[2*4+3] = gl_to_m(z);
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::scale(GLfixed x, GLfixed y, GLfixed z)
{
  Mfixed m[16];
  matrixfx_identity(m);
  m[0*4+0] = gl_to_m(x);
  m[1*4+1] = gl_to_m(y);
  m[2*4+2] = gl_to_m(z);
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::rotate(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  // Normalize the angle
  angle = angle - gl_fpfromi((gl_fptoi(angle) / 360) * 360);
  if(angle < 0)
    angle += gl_fpfromi(360);
  // Get sin and cos from lookup table
  Mfixed iSin = fpSin_[gl_fptoi(angle) % 360];
  Mfixed iCos = fpCos_[gl_fptoi(angle) % 360];

  // Convert from gl fixed to m fixed
  x = gl_to_m(x);
  y = gl_to_m(y);
  z = gl_to_m(z);

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
      Mfixed m[16];
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
      Mfixed m[16];
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
      Mfixed m[16];
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
      Mfixed iMinCos = m_fpfromi(1) - iCos;
      Mfixed m[16];
      m[0*4+0] = m_fpmul(m_fpmul(x, x), iMinCos) + iCos;
      m[0*4+1] = m_fpmul(m_fpmul(x, y), iMinCos) - m_fpmul(z, iSin);
      m[0*4+2] = m_fpmul(m_fpmul(x, z), iMinCos) + m_fpmul(y, iSin);
      m[0*4+3] = m_fpfromi(0);
      m[1*4+0] = m_fpmul(m_fpmul(y, x), iMinCos) + m_fpmul(z, iSin);
      m[1*4+1] = m_fpmul(m_fpmul(y, y), iMinCos) + iCos;
      m[1*4+2] = m_fpmul(m_fpmul(y, z), iMinCos) - m_fpmul(x, iSin);
      m[1*4+3] = m_fpfromi(0);
      m[2*4+0] = m_fpmul(m_fpmul(z, x), iMinCos) - m_fpmul(y, iSin);
      m[2*4+1] = m_fpmul(m_fpmul(z, y), iMinCos) + m_fpmul(x, iSin);
      m[2*4+2] = m_fpmul(m_fpmul(z, z), iMinCos) + iCos;
      m[2*4+3] = m_fpfromi(0);
      m[3*4+0] = m_fpfromi(0);
      m[3*4+1] = m_fpfromi(0);
      m[3*4+2] = m_fpfromi(0);
      m[3*4+3] = m_fpfromi(1);
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
  Mfixed mtemp[16];

  mtemp[0*4+0] = m_fpmul(matrix[0*4+0], m.matrix[0*4+0]) + m_fpmul(matrix[0*4+1], m.matrix[1*4+0]) + m_fpmul(matrix[0*4+2], m.matrix[2*4+0]) + m_fpmul(matrix[0*4+3], m.matrix[3*4+0]);
  mtemp[0*4+1] = m_fpmul(matrix[0*4+0], m.matrix[0*4+1]) + m_fpmul(matrix[0*4+1], m.matrix[1*4+1]) + m_fpmul(matrix[0*4+2], m.matrix[2*4+1]) + m_fpmul(matrix[0*4+3], m.matrix[3*4+1]);
  mtemp[0*4+2] = m_fpmul(matrix[0*4+0], m.matrix[0*4+2]) + m_fpmul(matrix[0*4+1], m.matrix[1*4+2]) + m_fpmul(matrix[0*4+2], m.matrix[2*4+2]) + m_fpmul(matrix[0*4+3], m.matrix[3*4+2]);
  mtemp[0*4+3] = m_fpmul(matrix[0*4+0], m.matrix[0*4+3]) + m_fpmul(matrix[0*4+1], m.matrix[1*4+3]) + m_fpmul(matrix[0*4+2], m.matrix[2*4+3]) + m_fpmul(matrix[0*4+3], m.matrix[3*4+3]);

  mtemp[1*4+0] = m_fpmul(matrix[1*4+0], m.matrix[0*4+0]) + m_fpmul(matrix[1*4+1], m.matrix[1*4+0]) + m_fpmul(matrix[1*4+2], m.matrix[2*4+0]) + m_fpmul(matrix[1*4+3], m.matrix[3*4+0]);
  mtemp[1*4+1] = m_fpmul(matrix[1*4+0], m.matrix[0*4+1]) + m_fpmul(matrix[1*4+1], m.matrix[1*4+1]) + m_fpmul(matrix[1*4+2], m.matrix[2*4+1]) + m_fpmul(matrix[1*4+3], m.matrix[3*4+1]);
  mtemp[1*4+2] = m_fpmul(matrix[1*4+0], m.matrix[0*4+2]) + m_fpmul(matrix[1*4+1], m.matrix[1*4+2]) + m_fpmul(matrix[1*4+2], m.matrix[2*4+2]) + m_fpmul(matrix[1*4+3], m.matrix[3*4+2]);
  mtemp[1*4+3] = m_fpmul(matrix[1*4+0], m.matrix[0*4+3]) + m_fpmul(matrix[1*4+1], m.matrix[1*4+3]) + m_fpmul(matrix[1*4+2], m.matrix[2*4+3]) + m_fpmul(matrix[1*4+3], m.matrix[3*4+3]);

  mtemp[2*4+0] = m_fpmul(matrix[2*4+0], m.matrix[0*4+0]) + m_fpmul(matrix[2*4+1], m.matrix[1*4+0]) + m_fpmul(matrix[2*4+2], m.matrix[2*4+0]) + m_fpmul(matrix[2*4+3], m.matrix[3*4+0]);
  mtemp[2*4+1] = m_fpmul(matrix[2*4+0], m.matrix[0*4+1]) + m_fpmul(matrix[2*4+1], m.matrix[1*4+1]) + m_fpmul(matrix[2*4+2], m.matrix[2*4+1]) + m_fpmul(matrix[2*4+3], m.matrix[3*4+1]);
  mtemp[2*4+2] = m_fpmul(matrix[2*4+0], m.matrix[0*4+2]) + m_fpmul(matrix[2*4+1], m.matrix[1*4+2]) + m_fpmul(matrix[2*4+2], m.matrix[2*4+2]) + m_fpmul(matrix[2*4+3], m.matrix[3*4+2]);
  mtemp[2*4+3] = m_fpmul(matrix[2*4+0], m.matrix[0*4+3]) + m_fpmul(matrix[2*4+1], m.matrix[1*4+3]) + m_fpmul(matrix[2*4+2], m.matrix[2*4+3]) + m_fpmul(matrix[2*4+3], m.matrix[3*4+3]);

  mtemp[3*4+0] = m_fpmul(matrix[3*4+0], m.matrix[0*4+0]) + m_fpmul(matrix[3*4+1], m.matrix[1*4+0]) + m_fpmul(matrix[3*4+2], m.matrix[2*4+0]) + m_fpmul(matrix[3*4+3], m.matrix[3*4+0]);
  mtemp[3*4+1] = m_fpmul(matrix[3*4+0], m.matrix[0*4+1]) + m_fpmul(matrix[3*4+1], m.matrix[1*4+1]) + m_fpmul(matrix[3*4+2], m.matrix[2*4+1]) + m_fpmul(matrix[3*4+3], m.matrix[3*4+1]);
  mtemp[3*4+2] = m_fpmul(matrix[3*4+0], m.matrix[0*4+2]) + m_fpmul(matrix[3*4+1], m.matrix[1*4+2]) + m_fpmul(matrix[3*4+2], m.matrix[2*4+2]) + m_fpmul(matrix[3*4+3], m.matrix[3*4+2]);
  mtemp[3*4+3] = m_fpmul(matrix[3*4+0], m.matrix[0*4+3]) + m_fpmul(matrix[3*4+1], m.matrix[1*4+3]) + m_fpmul(matrix[3*4+2], m.matrix[2*4+3]) + m_fpmul(matrix[3*4+3], m.matrix[3*4+3]);

  matrixfx_copy(matrix, mtemp);

  return (*this);
}

//---------------------------------------------------------------------------
CMatrixFx &
CMatrixFx::operator*=(const Mfixed * m)
{
  Mfixed mtemp[16];

  mtemp[0*4+0] = m_fpmul(matrix[0*4+0], m[0*4+0]) + m_fpmul(matrix[0*4+1], m[1*4+0]) + m_fpmul(matrix[0*4+2], m[2*4+0]) + m_fpmul(matrix[0*4+3], m[3*4+0]);
  mtemp[0*4+1] = m_fpmul(matrix[0*4+0], m[0*4+1]) + m_fpmul(matrix[0*4+1], m[1*4+1]) + m_fpmul(matrix[0*4+2], m[2*4+1]) + m_fpmul(matrix[0*4+3], m[3*4+1]);
  mtemp[0*4+2] = m_fpmul(matrix[0*4+0], m[0*4+2]) + m_fpmul(matrix[0*4+1], m[1*4+2]) + m_fpmul(matrix[0*4+2], m[2*4+2]) + m_fpmul(matrix[0*4+3], m[3*4+2]);
  mtemp[0*4+3] = m_fpmul(matrix[0*4+0], m[0*4+3]) + m_fpmul(matrix[0*4+1], m[1*4+3]) + m_fpmul(matrix[0*4+2], m[2*4+3]) + m_fpmul(matrix[0*4+3], m[3*4+3]);

  mtemp[1*4+0] = m_fpmul(matrix[1*4+0], m[0*4+0]) + m_fpmul(matrix[1*4+1], m[1*4+0]) + m_fpmul(matrix[1*4+2], m[2*4+0]) + m_fpmul(matrix[1*4+3], m[3*4+0]);
  mtemp[1*4+1] = m_fpmul(matrix[1*4+0], m[0*4+1]) + m_fpmul(matrix[1*4+1], m[1*4+1]) + m_fpmul(matrix[1*4+2], m[2*4+1]) + m_fpmul(matrix[1*4+3], m[3*4+1]);
  mtemp[1*4+2] = m_fpmul(matrix[1*4+0], m[0*4+2]) + m_fpmul(matrix[1*4+1], m[1*4+2]) + m_fpmul(matrix[1*4+2], m[2*4+2]) + m_fpmul(matrix[1*4+3], m[3*4+2]);
  mtemp[1*4+3] = m_fpmul(matrix[1*4+0], m[0*4+3]) + m_fpmul(matrix[1*4+1], m[1*4+3]) + m_fpmul(matrix[1*4+2], m[2*4+3]) + m_fpmul(matrix[1*4+3], m[3*4+3]);

  mtemp[2*4+0] = m_fpmul(matrix[2*4+0], m[0*4+0]) + m_fpmul(matrix[2*4+1], m[1*4+0]) + m_fpmul(matrix[2*4+2], m[2*4+0]) + m_fpmul(matrix[2*4+3], m[3*4+0]);
  mtemp[2*4+1] = m_fpmul(matrix[2*4+0], m[0*4+1]) + m_fpmul(matrix[2*4+1], m[1*4+1]) + m_fpmul(matrix[2*4+2], m[2*4+1]) + m_fpmul(matrix[2*4+3], m[3*4+1]);
  mtemp[2*4+2] = m_fpmul(matrix[2*4+0], m[0*4+2]) + m_fpmul(matrix[2*4+1], m[1*4+2]) + m_fpmul(matrix[2*4+2], m[2*4+2]) + m_fpmul(matrix[2*4+3], m[3*4+2]);
  mtemp[2*4+3] = m_fpmul(matrix[2*4+0], m[0*4+3]) + m_fpmul(matrix[2*4+1], m[1*4+3]) + m_fpmul(matrix[2*4+2], m[2*4+3]) + m_fpmul(matrix[2*4+3], m[3*4+3]);

  mtemp[3*4+0] = m_fpmul(matrix[3*4+0], m[0*4+0]) + m_fpmul(matrix[3*4+1], m[1*4+0]) + m_fpmul(matrix[3*4+2], m[2*4+0]) + m_fpmul(matrix[3*4+3], m[3*4+0]);
  mtemp[3*4+1] = m_fpmul(matrix[3*4+0], m[0*4+1]) + m_fpmul(matrix[3*4+1], m[1*4+1]) + m_fpmul(matrix[3*4+2], m[2*4+1]) + m_fpmul(matrix[3*4+3], m[3*4+1]);
  mtemp[3*4+2] = m_fpmul(matrix[3*4+0], m[0*4+2]) + m_fpmul(matrix[3*4+1], m[1*4+2]) + m_fpmul(matrix[3*4+2], m[2*4+2]) + m_fpmul(matrix[3*4+3], m[3*4+2]);
  mtemp[3*4+3] = m_fpmul(matrix[3*4+0], m[0*4+3]) + m_fpmul(matrix[3*4+1], m[1*4+3]) + m_fpmul(matrix[3*4+2], m[2*4+3]) + m_fpmul(matrix[3*4+3], m[3*4+3]);

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
#else
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
