#ifndef PS2_MATRIX_H
#define PS2_MATRIX_H


#include "GL/gl.h"
#include "vhl/matrix.h"


class CPS2Matrix
{
public:
  inline CPS2Matrix(){}
  CPS2Matrix(const CPS2Matrix & m);
  CPS2Matrix(const TMatrix4x4<GLfloat> & m);

  inline void transform(const GLfloat * v1, GLfloat * v2);

public:
  GLfloat matrix[16];
};


#include "matrix.inl"


#endif
