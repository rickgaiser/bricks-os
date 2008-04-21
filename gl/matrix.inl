#include "matrix.h"
#include "string.h"
typedef unsigned int wint_t;
#include <math.h>


#define matrixf_copy(mto, mfrom) \
mto[0*4+0] = mfrom[0*4+0]; mto[0*4+1] = mfrom[0*4+1]; mto[0*4+2] = mfrom[0*4+2]; mto[0*4+3] = mfrom[0*4+3]; \
mto[1*4+0] = mfrom[1*4+0]; mto[1*4+1] = mfrom[1*4+1]; mto[1*4+2] = mfrom[1*4+2]; mto[1*4+3] = mfrom[1*4+3]; \
mto[2*4+0] = mfrom[2*4+0]; mto[2*4+1] = mfrom[2*4+1]; mto[2*4+2] = mfrom[2*4+2]; mto[2*4+3] = mfrom[2*4+3]; \
mto[3*4+0] = mfrom[3*4+0]; mto[3*4+1] = mfrom[3*4+1]; mto[3*4+2] = mfrom[3*4+2]; mto[3*4+3] = mfrom[3*4+3]
//memcpy(mto, mfrom, sizeof(GLfloat) * 16)
#define matrixf_clear(m) \
m[0*4+0] = 0; m[0*4+1] = 0; m[0*4+2] = 0; m[0*4+3] = 0; \
m[1*4+0] = 0; m[1*4+1] = 0; m[1*4+2] = 0; m[1*4+3] = 0; \
m[2*4+0] = 0; m[2*4+1] = 0; m[2*4+2] = 0; m[2*4+3] = 0; \
m[3*4+0] = 0; m[3*4+1] = 0; m[3*4+2] = 0; m[3*4+3] = 0
//memset(m, 0, sizeof(GLfloat) * 16)
#define matrixf_identity(m) \
m[0*4+0] = 1; m[0*4+1] = 0; m[0*4+2] = 0; m[0*4+3] = 0; \
m[1*4+0] = 0; m[1*4+1] = 1; m[1*4+2] = 0; m[1*4+3] = 0; \
m[2*4+0] = 0; m[2*4+1] = 0; m[2*4+2] = 1; m[2*4+3] = 0; \
m[3*4+0] = 0; m[3*4+1] = 0; m[3*4+2] = 0; m[3*4+3] = 1

#define matrixfx_copy(mto, mfrom) \
matrixf_copy(mto, mfrom)
//memcpy(mto, mfrom, sizeof(GLfixed) * 16)
#define matrixfx_clear(m) \
matrixf_clear(m)
//memset(m, 0, sizeof(GLfixed) * 16)
#define matrixfx_identity(m) \
m[0*4+0] = gl_fpfromi(1); m[0*4+1] = gl_fpfromi(0); m[0*4+2] = gl_fpfromi(0); m[0*4+3] = gl_fpfromi(0); \
m[1*4+0] = gl_fpfromi(0); m[1*4+1] = gl_fpfromi(1); m[1*4+2] = gl_fpfromi(0); m[1*4+3] = gl_fpfromi(0); \
m[2*4+0] = gl_fpfromi(0); m[2*4+1] = gl_fpfromi(0); m[2*4+2] = gl_fpfromi(1); m[2*4+3] = gl_fpfromi(0); \
m[3*4+0] = gl_fpfromi(0); m[3*4+1] = gl_fpfromi(0); m[3*4+2] = gl_fpfromi(0); m[3*4+3] = gl_fpfromi(1)


#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline
CMatrixF::CMatrixF(const GLfloat * m)
{
  matrixf_copy(matrix, m);
}

//---------------------------------------------------------------------------
inline
CMatrixF::CMatrixF(const CMatrixF & m)
{
  matrixf_copy(matrix, m.matrix);
}

//---------------------------------------------------------------------------
inline
CMatrixF::CMatrixF(GLfloat _m00, GLfloat _m01, GLfloat _m02, GLfloat _m03,
                   GLfloat _m10, GLfloat _m11, GLfloat _m12, GLfloat _m13,
                   GLfloat _m20, GLfloat _m21, GLfloat _m22, GLfloat _m23,
                   GLfloat _m30, GLfloat _m31, GLfloat _m32, GLfloat _m33)
{
  m00 = _m00; m01 = _m01; m02 = _m02; m03 = _m03;
  m10 = _m10; m11 = _m11; m12 = _m12; m13 = _m13;
  m20 = _m20; m21 = _m21; m22 = _m22; m23 = _m23;
  m30 = _m30; m31 = _m31; m32 = _m32; m33 = _m33;
}

//---------------------------------------------------------------------------
inline void
CMatrixF::clear()
{
  matrixf_clear(matrix);
}

//---------------------------------------------------------------------------
inline void
CMatrixF::loadIdentity()
{
  matrixf_identity(matrix);
}

//---------------------------------------------------------------------------
inline void
CMatrixF::translate(GLfloat * vec)
{
  translate(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
inline void
CMatrixF::scale(GLfloat * vec)
{
  scale(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
inline void
CMatrixF::rotate(GLfloat * angles)
{
  rotate(angles[0], angles[1], angles[2], angles[3]);
}

//---------------------------------------------------------------------------
inline void
CMatrixF::transform(const GLfloat * from, GLfloat * to)
{
  GLfloat x(from[0]);
  GLfloat y(from[1]);
  GLfloat z(from[2]);
  GLfloat w(from[3]);

  to[0] = m00 * x + m01 * y + m02 * z + m03 * w;
  to[1] = m10 * x + m11 * y + m12 * z + m13 * w;
  to[2] = m20 * x + m21 * y + m22 * z + m23 * w;
  to[3] = m30 * x + m31 * y + m32 * z + m33 * w;
}

//---------------------------------------------------------------------------
inline CMatrixF
CMatrixF::operator*(const CMatrixF & m)
{
  CMatrixF mReturn(*this);

  mReturn *= m;

  return mReturn;
}

//---------------------------------------------------------------------------
inline CMatrixF
CMatrixF::operator*(const GLfloat * m)
{
  CMatrixF mReturn(*this);

  mReturn *= m;

  return mReturn;
}

//---------------------------------------------------------------------------
inline CMatrixF &
CMatrixF::operator=(const CMatrixF & m)
{
  matrixf_copy(matrix, m.matrix);

  return(*this);
}

//---------------------------------------------------------------------------
inline CMatrixF &
CMatrixF::operator=(const GLfloat * m)
{
  matrixf_copy(matrix, m);

  return(*this);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline void
vecInverseF(GLfloat * vto, const GLfloat * vfrom)
{
  vto[0] = -vfrom[0];
  vto[1] = -vfrom[1];
  vto[2] = -vfrom[2];
  vto[3] = -vfrom[3];
}

//---------------------------------------------------------------------------
inline void
vecNormalizeF(GLfloat * vto, const GLfloat * vfrom)
{
  float norm, dnorm;

  norm = sqrt(vfrom[0]*vfrom[0] + vfrom[1]*vfrom[1] + vfrom[2]*vfrom[2]);

  if(norm > 0)
  {
    dnorm = 1.0 / norm;
    vto[0] = vfrom[0] * dnorm;
    vto[1] = vfrom[1] * dnorm;
    vto[2] = vfrom[2] * dnorm;
    vto[3] = vfrom[3];
  }
}

//-----------------------------------------------------------------------------
inline GLfloat
vecInnerProductF(const GLfloat * v0, const GLfloat * v1)
{
  return (v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2]);
}
#else // CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline
CMatrixFx::CMatrixFx(const GLfixed * m)
{
  matrixfx_copy(matrix, m);
}

//---------------------------------------------------------------------------
inline
CMatrixFx::CMatrixFx(const CMatrixFx & m)
{
  matrixfx_copy(matrix, m.matrix);
}

//---------------------------------------------------------------------------
inline
CMatrixFx::CMatrixFx(GLfixed _m00, GLfixed _m01, GLfixed _m02, GLfixed _m03,
                     GLfixed _m10, GLfixed _m11, GLfixed _m12, GLfixed _m13,
                     GLfixed _m20, GLfixed _m21, GLfixed _m22, GLfixed _m23,
                     GLfixed _m30, GLfixed _m31, GLfixed _m32, GLfixed _m33)
{
  m00 = _m00; m01 = _m01; m02 = _m02; m03 = _m03;
  m10 = _m10; m11 = _m11; m12 = _m12; m13 = _m13;
  m20 = _m20; m21 = _m21; m22 = _m22; m23 = _m23;
  m30 = _m30; m31 = _m31; m32 = _m32; m33 = _m33;
}

//---------------------------------------------------------------------------
inline void
CMatrixFx::clear()
{
  matrixfx_clear(matrix);
}

//---------------------------------------------------------------------------
inline void
CMatrixFx::loadIdentity()
{
  matrixfx_identity(matrix);
}

//---------------------------------------------------------------------------
inline void
CMatrixFx::translate(GLfixed * vec)
{
  translate(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
inline void
CMatrixFx::scale(GLfixed * vec)
{
  scale(vec[0], vec[1], vec[2]);
}

//---------------------------------------------------------------------------
inline void
CMatrixFx::rotate(GLfixed * angles)
{
  rotate(angles[0], angles[1], angles[2], angles[3]);
}

//---------------------------------------------------------------------------
inline void
CMatrixFx::transform(const GLfixed * from, GLfixed * to)
{
  GLfixed x(from[0]);
  GLfixed y(from[1]);
  GLfixed z(from[2]);
  GLfixed w(from[3]);

  to[0] = gl_fpmul(m00, x) + gl_fpmul(m01, y) + gl_fpmul(m02, z) + gl_fpmul(m03, w);
  to[1] = gl_fpmul(m10, x) + gl_fpmul(m11, y) + gl_fpmul(m12, z) + gl_fpmul(m13, w);
  to[2] = gl_fpmul(m20, x) + gl_fpmul(m21, y) + gl_fpmul(m22, z) + gl_fpmul(m23, w);
  to[3] = gl_fpmul(m30, x) + gl_fpmul(m31, y) + gl_fpmul(m32, z) + gl_fpmul(m33, w);
}

//---------------------------------------------------------------------------
inline CMatrixFx
CMatrixFx::operator*(const CMatrixFx & m)
{
  CMatrixFx mReturn(*this);

  mReturn *= m;

  return mReturn;
}

//---------------------------------------------------------------------------
inline CMatrixFx
CMatrixFx::operator*(const GLfixed * m)
{
  CMatrixFx mReturn(*this);

  mReturn *= m;

  return mReturn;
}

//---------------------------------------------------------------------------
inline CMatrixFx &
CMatrixFx::operator=(const CMatrixFx & m)
{
  matrixfx_copy(matrix, m.matrix);

  return(*this);
}

//---------------------------------------------------------------------------
inline CMatrixFx &
CMatrixFx::operator=(const GLfixed * m)
{
  matrixfx_copy(matrix, m);

  return(*this);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline void
vecInverseFx(GLfixed * vto, const GLfixed * vfrom)
{
  vto[0] = -vfrom[0];
  vto[1] = -vfrom[1];
  vto[2] = -vfrom[2];
  vto[3] = -vfrom[3];
}

//---------------------------------------------------------------------------
inline void
vecNormalizeFx(GLfixed * vto, const GLfixed * vfrom)
{
  GLfixed norm, dnorm;

  norm = gl_fpfromf(sqrt(gl_fptof(gl_fpmul(vfrom[0], vfrom[0]) + gl_fpmul(vfrom[1], vfrom[1]) + gl_fpmul(vfrom[2], vfrom[2]))));

  if(norm > 0)
  {
    dnorm = gl_fpdiv(gl_fpfromi(1), norm);
    vto[0] = gl_fpmul(vfrom[0], dnorm);
    vto[1] = gl_fpmul(vfrom[1], dnorm);
    vto[2] = gl_fpmul(vfrom[2], dnorm);
    vto[3] = vfrom[3];
  }
}

//-----------------------------------------------------------------------------
inline GLfixed
vecInnerProductFx(const GLfixed * v0, const GLfixed * v1)
{
  return (gl_fpmul(v0[0], v1[0]) + gl_fpmul(v0[1], v1[1]) + gl_fpmul(v0[2], v1[2]));
}
#endif // CONFIG_FPU
