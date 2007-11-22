#include "matrix.h"
#include "string.h"
//typedef unsigned int wint_t;
//#include <math.h>


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
//memcpy(mto, mfrom, sizeof(Mfixed) * 16)
#define matrixfx_clear(m) \
matrixf_clear(m)
//memset(m, 0, sizeof(Mfixed) * 16)
#define matrixfx_identity(m) \
m[0*4+0] = m_fpfromi(1); m[0*4+1] = m_fpfromi(0); m[0*4+2] = m_fpfromi(0); m[0*4+3] = m_fpfromi(0); \
m[1*4+0] = m_fpfromi(0); m[1*4+1] = m_fpfromi(1); m[1*4+2] = m_fpfromi(0); m[1*4+3] = m_fpfromi(0); \
m[2*4+0] = m_fpfromi(0); m[2*4+1] = m_fpfromi(0); m[2*4+2] = m_fpfromi(1); m[2*4+3] = m_fpfromi(0); \
m[3*4+0] = m_fpfromi(0); m[3*4+1] = m_fpfromi(0); m[3*4+2] = m_fpfromi(0); m[3*4+3] = m_fpfromi(1)


#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
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

  to[0] = matrix[0*4+0] * x + matrix[0*4+1] * y + matrix[0*4+2] * z + matrix[0*4+3] * w;
  to[1] = matrix[1*4+0] * x + matrix[1*4+1] * y + matrix[1*4+2] * z + matrix[1*4+3] * w;
  to[2] = matrix[2*4+0] * x + matrix[2*4+1] * y + matrix[2*4+2] * z + matrix[2*4+3] * w;
  to[3] = matrix[3*4+0] * x + matrix[3*4+1] * y + matrix[3*4+2] * z + matrix[3*4+3] * w;
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
#else
//---------------------------------------------------------------------------
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
  Mfixed x(gl_to_m(from[0]));
  Mfixed y(gl_to_m(from[1]));
  Mfixed z(gl_to_m(from[2]));
  Mfixed w(gl_to_m(from[3]));

  to[0] = m_to_gl(m_fpmul(matrix[0*4+0], x) + m_fpmul(matrix[0*4+1], y) + m_fpmul(matrix[0*4+2], z) + m_fpmul(matrix[0*4+3], w));
  to[1] = m_to_gl(m_fpmul(matrix[1*4+0], x) + m_fpmul(matrix[1*4+1], y) + m_fpmul(matrix[1*4+2], z) + m_fpmul(matrix[1*4+3], w));
  to[2] = m_to_gl(m_fpmul(matrix[2*4+0], x) + m_fpmul(matrix[2*4+1], y) + m_fpmul(matrix[2*4+2], z) + m_fpmul(matrix[2*4+3], w));
  to[3] = m_to_gl(m_fpmul(matrix[3*4+0], x) + m_fpmul(matrix[3*4+1], y) + m_fpmul(matrix[3*4+2], z) + m_fpmul(matrix[3*4+3], w));
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
CMatrixFx::operator*(const Mfixed * m)
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
CMatrixFx::operator=(const Mfixed * m)
{
  matrixfx_copy(matrix, m);

  return(*this);
}
#endif // CONFIG_FPU
