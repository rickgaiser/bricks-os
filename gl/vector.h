#ifndef GL_VECTOR_H
#define GL_VECTOR_H


#include "GLES/gl.h"


//---------------------------------------------------------------------------
typedef struct CVectorF
{
#ifdef __cplusplus
public:
  CVectorF(){}
  CVectorF(const GLfloat * vec);
  CVectorF(const CVectorF & vec);
  CVectorF(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w);

  CVectorF   operator+ (const CVectorF & vec);
  CVectorF   operator- (const CVectorF & vec);
  CVectorF   operator* (GLfloat s);
  CVectorF   operator/ (GLfloat s);
  CVectorF   getInverted();
  CVectorF   getNormalized(); // Warning: uses sqrt==slow
  CVectorF   getCrossProduct(const CVectorF & vec); // Reflection

  CVectorF & operator+=(const CVectorF & vec);
  CVectorF & operator-=(const CVectorF & vec);
  CVectorF & operator*=(GLfloat s);
  CVectorF & operator/=(GLfloat s);
  CVectorF & invert();
  CVectorF & normalize(); // Warning: uses sqrt==slow
  CVectorF & crossProduct(const CVectorF & vec); // Reflection

  CVectorF & operator= (const CVectorF & vec);

  GLfloat length(); // Warning: uses sqrt==slow
  GLfloat dotProduct(const CVectorF & vec);

#endif // __cplusplus

  union
  {
    GLfloat v[4];
#ifdef __cplusplus
    struct
    {
      GLfloat x;
      GLfloat y;
      GLfloat z;
      GLfloat w;
    };
#endif // __cplusplus
  };
};


#include "vector.inl"


#endif // GL_VECTOR_H
