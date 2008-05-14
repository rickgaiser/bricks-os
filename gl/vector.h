#ifndef GL_VECTOR_H
#define GL_VECTOR_H


//---------------------------------------------------------------------------
template <class T>
class TVector4;

//---------------------------------------------------------------------------
template <class T>
class TVector3
{
public:
  TVector3(){}
  TVector3(const T * vec);
  TVector3(const TVector3 & vec);
  TVector3(const TVector4<T> & vec);
  TVector3(T _x, T _y, T _z);

  TVector3 & operator= (const T * vec);
  TVector3 & operator= (const TVector3 & vec);
  TVector3 & operator= (const TVector4<T> & vec);

  TVector3   operator+ (const TVector3 & vec) const;
  TVector3   operator- (const TVector3 & vec) const;
  TVector3   operator* (T s) const;
  TVector3   operator/ (T s) const;
  TVector3   getInverted() const;
  TVector3   getNormalized() const; // Warning: uses sqrt==slow
  TVector3   getCrossProduct(const TVector3 & vec) const; // Reflection

  TVector3 & operator+=(const TVector3 & vec);
  TVector3 & operator-=(const TVector3 & vec);
  TVector3 & operator*=(T s);
  TVector3 & operator/=(T s);
  TVector3 & invert();
  TVector3 & normalize(); // Warning: uses sqrt==slow
  TVector3 & crossProduct(const TVector3 & vec); // Reflection

  T length() const; // Warning: uses sqrt==slow
  T dotProduct(const TVector3 & vec) const;

public:
  T x;
  T y;
  T z;
};

//---------------------------------------------------------------------------
template <class T>
class TVector4
{
public:
  TVector4(){}
  TVector4(const T * vec);
  TVector4(const TVector3<T> & vec);
  TVector4(const TVector4 & vec);
  TVector4(T _x, T _y, T _z, T _w);

  TVector4 & operator= (const T * vec);
  TVector4 & operator= (const TVector3<T> & vec);
  TVector4 & operator= (const TVector4 & vec);

  TVector4   operator+ (const TVector4 & vec) const;
  TVector4   operator- (const TVector4 & vec) const;
  TVector4   operator* (T s) const;
  TVector4   operator/ (T s) const;
  TVector4   getInverted() const;
  TVector4   getNormalized() const; // Warning: uses sqrt==slow
  TVector4   getCrossProduct(const TVector4 & vec) const; // Reflection

  TVector4 & operator+=(const TVector4 & vec);
  TVector4 & operator-=(const TVector4 & vec);
  TVector4 & operator*=(T s);
  TVector4 & operator/=(T s);
  TVector4 & invert();
  TVector4 & normalize(); // Warning: uses sqrt==slow
  TVector4 & crossProduct(const TVector4 & vec); // Reflection

  T length() const; // Warning: uses sqrt==slow
  T dotProduct(const TVector4 & vec) const;

public:
  T x;
  T y;
  T z;
  T w;
};


#include "vector.inl"


#endif // GL_VECTOR_H
