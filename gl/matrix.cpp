#include "matrix.h"
#include "string.h"
typedef unsigned int wint_t;
#include <math.h>


bool      CMatrixF::bInitialized_(false);
GLfloat   CMatrixF::fpSin_[360];
GLfloat   CMatrixF::fpCos_[360];

bool      CMatrixFx::bInitialized_(false);
GLfixed   CMatrixFx::fpSin_[360];
GLfixed   CMatrixFx::fpCos_[360];


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

  clear();
}

//---------------------------------------------------------------------------
CMatrixF::~CMatrixF()
{
}

//---------------------------------------------------------------------------
void
CMatrixF::clear()
{
  memset(matrix, 0, sizeof(GLfloat) * 16);
}

//---------------------------------------------------------------------------
void
CMatrixF::loadIdentity()
{
  clear();
  matrix[0][0] = 1.0f;
  matrix[1][1] = 1.0f;
  matrix[2][2] = 1.0f;
  matrix[3][3] = 1.0f;
}

//---------------------------------------------------------------------------
void
CMatrixF::translate(GLfloat x, GLfloat y, GLfloat z)
{
  CMatrixF m;
  m.loadIdentity();
  m.matrix[0][3] = x;
  m.matrix[1][3] = y;
  m.matrix[2][3] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixF::translate(GLfloat * vec)
{
  translate(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
void
CMatrixF::scale(GLfloat x, GLfloat y, GLfloat z)
{
  CMatrixF m;
  m.loadIdentity();
  m.matrix[0][0] = x;
  m.matrix[1][1] = y;
  m.matrix[2][2] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixF::scale(GLfloat * vec)
{
  scale(vec[0], vec[1], vec[2]);
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
      CMatrixF rot;
      rot.loadIdentity();
      rot.matrix[1][1] = iCos;
      rot.matrix[1][2] = -iSin;
      rot.matrix[2][1] = iSin;
      rot.matrix[2][2] = iCos;
      *this *= rot;
      break;
    }
    case 0x02:
    {
      // Y Rotation only
      CMatrixF rot;
      rot.loadIdentity();
      rot.matrix[0][0] = iCos;
      rot.matrix[0][2] = iSin;
      rot.matrix[2][0] = -iSin;
      rot.matrix[2][2] = iCos;
      *this *= rot;
      break;
    }
    case 0x04:
    {
      // Z Rotation only
      CMatrixF rot;
      rot.loadIdentity();
      rot.matrix[0][0] = iCos;
      rot.matrix[0][1] = -iSin;
      rot.matrix[1][0] = iSin;
      rot.matrix[1][1] = iCos;
      *this *= rot;
      break;
    }
    default:
    {
/*
      // Mixed Rotation
      CMatrixF rot;
      rot.loadIdentity();
      x = m_to_s(x);
      y = m_to_s(y);
      z = m_to_s(z);
      rot.matrix[0][0] = s_to_m(s_fpmul(x, x) + s_fpmul(iCos, s_fpfromi(1) - s_fpmul(x, x)));
      rot.matrix[1][0] = s_to_m(s_fpmul(s_fpmul(x, y), (s_fpfromi(1) - iCos) - s_fpmul(z, iSin)));
      rot.matrix[2][0] = s_to_m(s_fpmul(s_fpmul(z, x), (s_fpfromi(1) - iCos) + s_fpmul(y, iSin)));
      rot.matrix[0][1] = s_to_m(s_fpmul(s_fpmul(x, y), (s_fpfromi(1) - iCos) + s_fpmul(z, iSin)));
      rot.matrix[1][1] = s_to_m(s_fpmul(y, y) + s_fpmul(iCos, s_fpfromi(1) - s_fpmul(y, y)));
      rot.matrix[2][1] = s_to_m(s_fpmul(s_fpmul(y, z), (s_fpfromi(1) - iCos) - s_fpmul(x, iSin)));
      rot.matrix[0][2] = s_to_m(s_fpmul(s_fpmul(y, z), (s_fpfromi(1) - iCos) - s_fpmul(x, iSin)));
      rot.matrix[1][2] = s_to_m(s_fpmul(s_fpmul(y, z), (s_fpfromi(1) - iCos) + s_fpmul(x, iSin)));
      rot.matrix[2][2] = s_to_m(s_fpmul(z, z) + s_fpmul(iCos, s_fpfromi(1) - s_fpmul(z, z)));
      rot.matrix[3][3] = m_fpfromi(1);
      *this *= rot;
*/
    }
  };
}

//---------------------------------------------------------------------------
void
CMatrixF::rotate(GLfloat * angles)
{
  rotate(angles[0], angles[1], angles[2], angles[3]);
}

//---------------------------------------------------------------------------
void
CMatrixF::transform(const GLfloat * from, GLfloat * to)
{
  GLfloat x(from[0]);
  GLfloat y(from[1]);
  GLfloat z(from[2]);
  GLfloat w(from[3]);
  to[0] = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3] * w;
  to[1] = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3] * w;
  to[2] = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z + matrix[2][3] * w;
  to[3] = matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3] * w;
}

//---------------------------------------------------------------------------
CMatrixF
CMatrixF::operator*(const CMatrixF & m)
{
  CMatrixF mReturn(*this);

  mReturn *= *this;

  return(mReturn);
}

//---------------------------------------------------------------------------
// 64 x operator*
// 48 x operator+
CMatrixF &
CMatrixF::operator*=(const CMatrixF & m)
{
  GLfloat mtemp[4][4];

  for(int iRow(0); iRow < 4; iRow++)
  {
    for(int iCol(0); iCol < 4; iCol++)
    {
      mtemp[iRow][iCol] = matrix[iRow][0] * m.matrix[0][iCol] +
                          matrix[iRow][1] * m.matrix[1][iCol] +
                          matrix[iRow][2] * m.matrix[2][iCol] +
                          matrix[iRow][3] * m.matrix[3][iCol];
    }
  }

  memcpy(matrix, mtemp, sizeof(GLfloat) * 16);

  return(*this);
}

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

  clear();
}

//---------------------------------------------------------------------------
CMatrixFx::~CMatrixFx()
{
}

//---------------------------------------------------------------------------
void
CMatrixFx::clear()
{
  memset(matrix, 0, sizeof(GLfixed) * 16);
}

//---------------------------------------------------------------------------
void
CMatrixFx::loadIdentity()
{
  clear();
  matrix[0][0] = m_fpfromi(1);
  matrix[1][1] = m_fpfromi(1);
  matrix[2][2] = m_fpfromi(1);
  matrix[3][3] = m_fpfromi(1);
}

//---------------------------------------------------------------------------
void
CMatrixFx::translate(GLfixed x, GLfixed y, GLfixed z)
{
  CMatrixFx m;
  m.loadIdentity();
  m.matrix[0][3] = x;
  m.matrix[1][3] = y;
  m.matrix[2][3] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::translate(GLfixed * vec)
{
  translate(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
void
CMatrixFx::scale(GLfixed x, GLfixed y, GLfixed z)
{
  CMatrixFx m;
  m.loadIdentity();
  m.matrix[0][0] = x;
  m.matrix[1][1] = y;
  m.matrix[2][2] = z;
  *this *= m;
}

//---------------------------------------------------------------------------
void
CMatrixFx::scale(GLfixed * vec)
{
  scale(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
void
CMatrixFx::rotate(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  // Normalize the angle
  angle = angle - m_fpfromi((m_fptoi(angle) / 360) * 360);
  if(angle < 0)
    angle += m_fpfromi(360);
  // Get sin and cos from lookup table
  GLfixed iSin = fpSin_[m_fptoi(angle) % 360];
  GLfixed iCos = fpCos_[m_fptoi(angle) % 360];

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
      CMatrixFx rot;
      rot.loadIdentity();
      rot.matrix[1][1] = iCos;
      rot.matrix[1][2] = -iSin;
      rot.matrix[2][1] = iSin;
      rot.matrix[2][2] = iCos;
      *this *= rot;
      break;
    }
    case 0x02:
    {
      // Y Rotation only
      CMatrixFx rot;
      rot.loadIdentity();
      rot.matrix[0][0] = iCos;
      rot.matrix[0][2] = iSin;
      rot.matrix[2][0] = -iSin;
      rot.matrix[2][2] = iCos;
      *this *= rot;
      break;
    }
    case 0x04:
    {
      // Z Rotation only
      CMatrixFx rot;
      rot.loadIdentity();
      rot.matrix[0][0] = iCos;
      rot.matrix[0][1] = -iSin;
      rot.matrix[1][0] = iSin;
      rot.matrix[1][1] = iCos;
      *this *= rot;
      break;
    }
    default:
    {
/*
      // Mixed Rotation
      CMatrixFx rot;
      rot.loadIdentity();
      rot.matrix[0][0] = (x * x) + iCos * (1.0f - (x * x);
      rot.matrix[1][0] = (x * y) * (1.0f - iCos) - (z * iSin);
      rot.matrix[2][0] = (z * x) * (1.0f - iCos) + (y * iSin));
      rot.matrix[0][1] = (x * y) * (1.0f - iCos) + (z * iSin));
      rot.matrix[1][1] = (y * y) + iCos * (1.0f - (y * y));
      rot.matrix[2][1] = (y * z) * (1.0f - iCos) - (x * iSin));
      rot.matrix[0][2] = (y * z) * (1.0f - iCos) - (x * iSin));
      rot.matrix[1][2] = (y * z) * (1.0f - iCos) + (x * iSin));
      rot.matrix[2][2] = (z * z) + iCos * (1.0f - (z * z));
      rot.matrix[3][3] = 1.0f;
      *this *= rot;
*/
    }
  };
}

//---------------------------------------------------------------------------
void
CMatrixFx::rotate(GLfixed * angles)
{
  rotate(angles[0], angles[1], angles[2], angles[3]);
}

//---------------------------------------------------------------------------
void
CMatrixFx::transform(const GLfixed * from, GLfixed * to)
{
  GLfixed x(from[0]);
  GLfixed y(from[1]);
  GLfixed z(from[2]);
  GLfixed w(from[3]);
  to[0] = m_fpmul(matrix[0][0], x) + m_fpmul(matrix[0][1], y) + m_fpmul(matrix[0][2], z) + m_fpmul(matrix[0][3], w);
  to[1] = m_fpmul(matrix[1][0], x) + m_fpmul(matrix[1][1], y) + m_fpmul(matrix[1][2], z) + m_fpmul(matrix[1][3], w);
  to[2] = m_fpmul(matrix[2][0], x) + m_fpmul(matrix[2][1], y) + m_fpmul(matrix[2][2], z) + m_fpmul(matrix[2][3], w);
  to[3] = m_fpmul(matrix[3][0], x) + m_fpmul(matrix[3][1], y) + m_fpmul(matrix[3][2], z) + m_fpmul(matrix[3][3], w);
}

//---------------------------------------------------------------------------
CMatrixFx
CMatrixFx::operator*(const CMatrixFx & m)
{
  CMatrixFx mReturn(*this);

  mReturn *= *this;

  return(mReturn);
}

//---------------------------------------------------------------------------
// 64 x operator*
// 48 x operator+
CMatrixFx &
CMatrixFx::operator*=(const CMatrixFx & m)
{
  GLfixed mtemp[4][4];

  for(int iRow(0); iRow < 4; iRow++)
  {
    for(int iCol(0); iCol < 4; iCol++)
    {
      mtemp[iRow][iCol] = m_fpmul(matrix[iRow][0], m.matrix[0][iCol]) +
                          m_fpmul(matrix[iRow][1], m.matrix[1][iCol]) +
                          m_fpmul(matrix[iRow][2], m.matrix[2][iCol]) +
                          m_fpmul(matrix[iRow][3], m.matrix[3][iCol]);
    }
  }

  memcpy(matrix, mtemp, sizeof(GLfixed) * 16);

  return(*this);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixF::CAGLESMatrixF()
 : matrixMode_(GL_MODELVIEW)
 , pCurrentMatrix_(&matrixModelView)
{
  zNear_ =   0.1f;
  zFar_  = 100.0f;
  zA_    = -((zFar_ + zNear_) / (zFar_ - zNear_));
  zB_    = -((2.0f * zFar_ * zNear_) / (zFar_ - zNear_));
}

//---------------------------------------------------------------------------
CAGLESMatrixF::~CAGLESMatrixF()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  CMatrixF m;

  zNear_ = zNear;
  zFar_  = zFar;
  zA_    = -((zFar + zNear) / (zFar - zNear));
  zB_    = -((2.0f * zFar * zNear) / (zFar - zNear));

  m.matrix[0][0] = (2.0f * zNear) / (right - left);
  m.matrix[0][1] = 0.0f;
  m.matrix[0][2] = (right + left) / (right - left);
  m.matrix[0][3] = 0.0f;

  m.matrix[1][0] = 0.0f;
  m.matrix[1][1] = (2.0f * zNear) / (top - bottom);
  m.matrix[1][2] = (top + bottom) / (top - bottom);
  m.matrix[1][3] = 0.0f;

  m.matrix[2][0] = 0.0f;
  m.matrix[2][1] = 0.0f;
  m.matrix[2][2] = zA_;
  m.matrix[2][3] = zB_;

  m.matrix[3][0] = 0.0f;
  m.matrix[3][1] = 0.0f;
  m.matrix[3][2] = -1.0f;
  m.matrix[3][3] = 0.0f;

  (*pCurrentMatrix_) *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);

  // FIXME
  matrixRotation.rotate(angle, x, y, z);
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

  // FIXME
  matrixRotation.loadIdentity();
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
  };
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixFx::CAGLESMatrixFx()
 : matrixMode_(GL_MODELVIEW)
 , pCurrentMatrix_(&matrixModelView)
{
  zNear_ = gl_fpfromf(  0.1f);
  zFar_  = gl_fpfromf(100.0f);
  zA_    = -gl_fpdiv((zFar_ + zNear_), (zFar_ - zNear_));
  zB_    = -gl_fpdiv((gl_fpmul(zFar_, zNear_) << 1), (zFar_ - zNear_));
}

//---------------------------------------------------------------------------
CAGLESMatrixFx::~CAGLESMatrixFx()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  CMatrixFx m;

  zNear_ = zNear;
  zFar_  = zFar;
  zA_    = -gl_fpdiv((zFar + zNear), (zFar - zNear));
  zB_    = -gl_fpdiv((gl_fpmul(zFar, zNear) << 1), (zFar - zNear));

  m.matrix[0][0] = gl_fpdiv((zNear << 1), (right - left));
  m.matrix[0][1] = gl_fpfromi(0);
  m.matrix[0][2] = gl_fpdiv((right + left), (right - left));
  m.matrix[0][3] = gl_fpfromi(0);

  m.matrix[1][0] = gl_fpfromi(0);
  m.matrix[1][1] = gl_fpdiv((zNear << 1), (top - bottom));
  m.matrix[1][2] = gl_fpdiv((top + bottom), (top - bottom));
  m.matrix[1][3] = gl_fpfromi(0);

  m.matrix[2][0] = gl_fpfromi(0);
  m.matrix[2][1] = gl_fpfromi(0);
  m.matrix[2][2] = zA_;
  m.matrix[2][3] = zB_;

  m.matrix[3][0] = gl_fpfromi(0);
  m.matrix[3][1] = gl_fpfromi(0);
  m.matrix[3][2] = gl_fpfromi(-1);
  m.matrix[3][3] = gl_fpfromi(0);

  (*pCurrentMatrix_) *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  pCurrentMatrix_->rotate(angle, x, y, z);

  // FIXME
  matrixRotation.rotate(angle, x, y, z);
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

  // FIXME
  matrixRotation.loadIdentity();
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
  };
}
