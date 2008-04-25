#ifndef CMATRIX_H
#define CMATRIX_H


#include "context.h"
#include "fixedPoint.h"
#include "asm/arch/config.h"


#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
typedef struct CMatrixF
{
#ifdef __cplusplus
public:
  CMatrixF(){};
  CMatrixF(const GLfloat * m);
  CMatrixF(const CMatrixF & m);
  CMatrixF(GLfloat _m00, GLfloat _m01, GLfloat _m02, GLfloat _m03,
           GLfloat _m10, GLfloat _m11, GLfloat _m12, GLfloat _m13,
           GLfloat _m20, GLfloat _m21, GLfloat _m22, GLfloat _m23,
           GLfloat _m30, GLfloat _m31, GLfloat _m32, GLfloat _m33);

  void clear();
  void loadIdentity();

  void translate(GLfloat x, GLfloat y, GLfloat z);
  void translate(GLfloat * vec);

  void scale    (GLfloat x, GLfloat y, GLfloat z);
  void scale    (GLfloat * vec);

  void rotate   (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  void rotate   (GLfloat * angles);

  void transform(const GLfloat * from, GLfloat * to);

  CMatrixF   operator* (const CMatrixF & m);
  CMatrixF   operator* (const GLfloat * m);

  CMatrixF & operator*=(const CMatrixF & m);
  CMatrixF & operator*=(const GLfloat * m);

  CMatrixF & operator= (const CMatrixF & m);
  CMatrixF & operator= (const GLfloat * m);
#endif // __cplusplus

  union
  {
    GLfloat matrix[16];
#ifdef __cplusplus
    struct
    {
      GLfloat m00, m01, m02, m03;
      GLfloat m10, m11, m12, m13;
      GLfloat m20, m21, m22, m23;
      GLfloat m30, m31, m32, m33;
    };
#endif // __cplusplus
  };
};

//---------------------------------------------------------------------------
void vecInverseF(GLfloat * vto, const GLfloat * vfrom);
void vecNormalizeF(GLfloat * vto, const GLfloat * vfrom);
GLfloat vecInnerProductF(const GLfloat * v0, const GLfloat * v1);

#else // CONFIG_FPU
//---------------------------------------------------------------------------
typedef struct CMatrixFx
{
#ifdef __cplusplus
public:
  CMatrixFx(){};
  CMatrixFx(const GLfixed * m);
  CMatrixFx(const CMatrixFx & m);
  CMatrixFx(GLfixed _m00, GLfixed _m01, GLfixed _m02, GLfixed _m03,
            GLfixed _m10, GLfixed _m11, GLfixed _m12, GLfixed _m13,
            GLfixed _m20, GLfixed _m21, GLfixed _m22, GLfixed _m23,
            GLfixed _m30, GLfixed _m31, GLfixed _m32, GLfixed _m33);

  void clear();
  void loadIdentity();

  void translate(GLfixed x, GLfixed y, GLfixed z);
  void translate(GLfixed * vec);

  void scale    (GLfixed x, GLfixed y, GLfixed z);
  void scale    (GLfixed * vec);

  void rotate   (GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
  void rotate   (GLfixed * angles);

  void transform(const GLfixed * from, GLfixed * to);

  CMatrixFx   operator* (const CMatrixFx & m);
  CMatrixFx   operator* (const GLfixed * m);

  CMatrixFx & operator*=(const CMatrixFx & m);
  CMatrixFx & operator*=(const GLfixed * m);

  CMatrixFx & operator= (const CMatrixFx & m);
  CMatrixFx & operator= (const GLfixed * m);
#endif // __cplusplus

  union
  {
    GLfixed matrix[16];
#ifdef __cplusplus
    struct
    {
      GLfixed m00, m01, m02, m03;
      GLfixed m10, m11, m12, m13;
      GLfixed m20, m21, m22, m23;
      GLfixed m30, m31, m32, m33;
    };
#endif // __cplusplus
  };
};

//---------------------------------------------------------------------------
void vecInverseFx(GLfixed * vto, const GLfixed * vfrom);
void vecNormalizeFx(GLfixed * vto, const GLfixed * vfrom);
GLfixed vecInnerProductFx(const GLfixed * v0, const GLfixed * v1);
#endif // CONFIG_FPU


#include "matrix.inl"


#endif
