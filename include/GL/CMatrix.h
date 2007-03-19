#ifndef CMATRIX_H
#define CMATRIX_H


#include "fixedPoint.h"


#define MATRIX_PRECISION   10
#define m_fpfromi(i)   fpfromi(MATRIX_PRECISION,i)
#define m_fptoi(i)     fptoi(MATRIX_PRECISION,i)
#define m_fpfromf(i)   fpfromf(MATRIX_PRECISION,i)
#define m_fptof(i)     fptof(MATRIX_PRECISION,i)
#define m_fpmul(i1,i2) fpmul(MATRIX_PRECISION,i1,i2)
#define m_fpdiv(i1,i2) fpdiv(MATRIX_PRECISION,i1,i2)


class CMatrix
{
public:
  CMatrix();
  virtual ~CMatrix();

  inline void      clear();
  inline void      loadIdentity();

  inline void      translate(fxpoint_t x, fxpoint_t y, fxpoint_t z);
  inline void      translate(fxpoint_t * vec);

  inline void      scale    (fxpoint_t x, fxpoint_t y, fxpoint_t z);
  inline void      scale    (fxpoint_t * vec);

  inline void      rotate   (fxpoint_t angle, fxpoint_t x, fxpoint_t y, fxpoint_t z);
  inline void      rotate   (fxpoint_t * angles);

  inline void      transform(const fxpoint_t * from, fxpoint_t * to);

  inline CMatrix   operator* (const CMatrix & m);
  inline CMatrix & operator*=(const CMatrix & m);

private:
  fxpoint_t matrix[4][4];
  // [ROW][COLUMN]
  // 00, 01, 02, 03
  // 10, 11, 12, 13
  // 20, 21, 22, 23
  // 30, 31, 32, 33

  static bool bInitialized_;
  static fxpoint_t fpSin_[360];
  static fxpoint_t fpCos_[360];
};


#include "CMatrix.icc"


#endif
