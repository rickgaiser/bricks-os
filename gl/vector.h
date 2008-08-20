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
  // Constructors
  TVector3(){}
  TVector3(const T * vec);
  TVector3(const TVector3 & vec);
  TVector3(const TVector4<T> & vec);
  TVector3(T _x, T _y, T _z);

  // Accessor
  T & operator[](uint8_t item);
  T   operator[](uint8_t item) const;

  TVector3 & operator= (const T * vec);
  TVector3 & operator= (const TVector3 & vec);
  TVector3 & operator= (const TVector4<T> & vec);

  TVector3   operator+ (const TVector3 & vec) const;
  TVector3   operator- (const TVector3 & vec) const;
  TVector3   operator* (const TVector3 & vec) const;
  TVector3   operator/ (const TVector3 & vec) const;
  TVector3   operator* (T s) const;
  TVector3   operator/ (T s) const;
  TVector3   getInverted() const;
  TVector3   getNormalized() const; // Warning: uses sqrt==slow
  TVector3   getCrossProduct(const TVector3 & vec) const;
  TVector3   getReflection(const TVector3 & vec) const;

  TVector3 & operator+=(const TVector3 & vec);
  TVector3 & operator-=(const TVector3 & vec);
  TVector3 & operator*=(const TVector3 & vec);
  TVector3 & operator/=(const TVector3 & vec);
  TVector3 & operator*=(T s);
  TVector3 & operator/=(T s);
  TVector3 & invert();
  TVector3 & normalize(); // Warning: uses sqrt==slow
  TVector3 & crossProduct(const TVector3 & vec);
  TVector3 & reflection(const TVector3 & vec);

  T length() const; // Warning: uses sqrt==slow
  T dotProduct(const TVector3 & vec) const;

public:
  union
  {
    T vector[3];
    struct
    {
      T x;
      T y;
      T z;
    };
  };
};

//---------------------------------------------------------------------------
template <class T>
class TVector4
{
public:
  // Constructors
  TVector4(){}
  TVector4(const T * vec);
  TVector4(const TVector3<T> & vec);
  TVector4(const TVector4 & vec);
  TVector4(T _x, T _y, T _z, T _w);

  // Accessor
  T & operator[](uint8_t item);
  T   operator[](uint8_t item) const;

  TVector4 & operator= (const T * vec);
  TVector4 & operator= (const TVector3<T> & vec);
  TVector4 & operator= (const TVector4 & vec);

  TVector4   operator+ (const TVector4 & vec) const;
  TVector4   operator- (const TVector4 & vec) const;
  TVector4   operator* (const TVector4 & vec) const;
  TVector4   operator/ (const TVector4 & vec) const;
  TVector4   operator* (T s) const;
  TVector4   operator/ (T s) const;
  TVector4   getInverted() const;
  TVector4   getNormalized() const; // Warning: uses sqrt==slow
  TVector4   getCrossProduct(const TVector4 & vec) const;
  TVector4   getReflection(const TVector4 & vec) const;

  TVector4 & operator+=(const TVector4 & vec);
  TVector4 & operator-=(const TVector4 & vec);
  TVector4 & operator*=(const TVector4 & vec);
  TVector4 & operator/=(const TVector4 & vec);
  TVector4 & operator*=(T s);
  TVector4 & operator/=(T s);
  TVector4 & invert();
  TVector4 & normalize(); // Warning: uses sqrt==slow
  TVector4 & crossProduct(const TVector4 & vec);
  TVector4 & reflection(const TVector4 & vec);

  T length() const; // Warning: uses sqrt==slow
  T dotProduct(const TVector4 & vec) const;

public:
  union
  {
    T vector[4];
    struct
    {
      T x;
      T y;
      T z;
      T w;
    };
  };
};


#include "vector.inl"


#endif // GL_VECTOR_H
