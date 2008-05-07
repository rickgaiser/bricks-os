#include "vector.h"
typedef unsigned int wint_t;
#include "math.h"


//---------------------------------------------------------------------------
template <class T>
inline
TVector<T>::TVector(const T * vec)
 : x(vec[0]), y(vec[1]), z(vec[2]), w(vec[3])
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector<T>::TVector(const TVector<T> & vec)
 : x(vec.x), y(vec.y), z(vec.z), w(vec.w)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector<T>::TVector(T _x, T _y, T _z, T _w)
 : x(_x), y(_y), z(_z), w(_w)
{
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T>
TVector<T>::operator+(const TVector & vec) const
{
  TVector rv(*this);

  rv += vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T>
TVector<T>::operator-(const TVector & vec) const
{
  TVector rv(*this);

  rv -= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T>
TVector<T>::operator*(T s) const
{
  TVector rv(*this);

  rv *= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T>
TVector<T>::operator/(T s) const
{
  TVector rv(*this);

  rv /= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T>
TVector<T>::getInverted() const
{
  TVector rv(*this);

  rv.invert();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T>
TVector<T>::getNormalized() const
{
  TVector rv(*this);

  rv.normalize();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T>
TVector<T>::getCrossProduct(const TVector & vec) const
{
  TVector rv(*this);

  rv.crossProduct(vec);

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T> &
TVector<T>::operator+=(const TVector & vec)
{
  x += vec.x;
  y += vec.y;
  z += vec.z;
  w += vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T> &
TVector<T>::operator-=(const TVector & vec)
{
  x -= vec.x;
  y -= vec.y;
  z -= vec.z;
  w -= vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T> &
TVector<T>::operator*=(const T s)
{
  x *= s;
  y *= s;
  z *= s;
  w *= s;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T> &
TVector<T>::operator/=(const T s)
{
  if(s != 0)
  {
    T s2 = 1.0f / s;

    x *= s2;
    y *= s2;
    z *= s2;
    w *= s2;
  }

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T> &
TVector<T>::invert()
{
  x = 0 - x;
  y = 0 - y;
  z = 0 - z;
  w = 0 - w;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T> &
TVector<T>::normalize()
{
  return *this /= length();
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T> &
TVector<T>::crossProduct(const TVector & vec)
{
  return (*this -= vec * (this->dotProduct(vec) * 2.0f));
}

//---------------------------------------------------------------------------
template <class T>
inline TVector<T> &
TVector<T>::operator= (const TVector & vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline T
TVector<T>::length() const
{
  return sqrt(x*x + y*y + z*z);
}

//---------------------------------------------------------------------------
template <class T>
inline T
TVector<T>::dotProduct(const TVector & vec) const
{
  return (x*vec.x + y*vec.y + z*vec.z);
}
