#ifndef PS2_VU0_H
#define PS2_VU0_H


#include "../../../../gl/matrix.h"


namespace VU0
{


  class CMatrix4x4
   : public TMatrix4x4<GLfloat>
  {
  public:
    // Constructors
    CMatrix4x4(){}
    CMatrix4x4(const TMatrix4x4<GLfloat> & m);
    CMatrix4x4(const GLfloat * m);
    CMatrix4x4(GLfloat _m00, GLfloat _m01, GLfloat _m02, GLfloat _m03,
               GLfloat _m10, GLfloat _m11, GLfloat _m12, GLfloat _m13,
               GLfloat _m20, GLfloat _m21, GLfloat _m22, GLfloat _m23,
               GLfloat _m30, GLfloat _m31, GLfloat _m32, GLfloat _m33);

    virtual TMatrix4x4<GLfloat> & operator*=(const GLfloat * m);

    virtual void transform4(const GLfloat * from, GLfloat * to);
    virtual void transform4list(const GLfloat * from, int fromInc, GLfloat * to, int toInc, int count);
  };


};


#endif
