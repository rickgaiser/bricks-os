#ifndef GL_VECTOR_H
#define GL_VECTOR_H


#include "GLES/gl.h"
#include "fixedPoint.h"


//---------------------------------------------------------------------------
template <class T>
class TVector
{
public:
  TVector(){}
  TVector(const T * vec);
  TVector(const TVector & vec);
  TVector(T _x, T _y, T _z, T _w);

  TVector   operator+ (const TVector & vec) const;
  TVector   operator- (const TVector & vec) const;
  TVector   operator* (T s) const;
  TVector   operator/ (T s) const;
  TVector   getInverted() const;
  TVector   getNormalized() const; // Warning: uses sqrt==slow
  TVector   getCrossProduct(const TVector & vec) const; // Reflection

  TVector & operator+=(const TVector & vec);
  TVector & operator-=(const TVector & vec);
  TVector & operator*=(T s);
  TVector & operator/=(T s);
  TVector & invert();
  TVector & normalize(); // Warning: uses sqrt==slow
  TVector & crossProduct(const TVector & vec); // Reflection

  TVector & operator= (const TVector & vec);

  T length() const; // Warning: uses sqrt==slow
  T dotProduct(const TVector & vec) const;

public:
  T x;
  T y;
  T z;
  T w;
};


#include "vector.inl"


#endif // GL_VECTOR_H
