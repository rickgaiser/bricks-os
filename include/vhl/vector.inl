#include "vector.h"
#include "math.h"


//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3(const T * vec)
 : x(vec[0]), y(vec[1]), z(vec[2])
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3(const TVector3 & vec)
 : x(vec.x), y(vec.y), z(vec.z)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3(const TVector4<T> & vec)
 : x(vec.x), y(vec.y), z(vec.z)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector3<T>::TVector3(T _x, T _y, T _z)
 : x(_x), y(_y), z(_z)
{
}

//---------------------------------------------------------------------------
template <class T>
inline T &
TVector3<T>::operator[](uint8_t item)
{
  static T * vector[3] = {&x, &y, &z};

  return *vector[item];
}

//---------------------------------------------------------------------------
template <class T>
inline const T &
TVector3<T>::operator[](uint8_t item) const
{
  static T * vector[3] = {&x, &y, &z};

  return *vector[item];
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator= (const T * vec)
{
  x = vec[0];
  y = vec[1];
  z = vec[2];

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator= (const TVector3 & vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator= (const TVector4<T> & vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator+(const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv += vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator-(const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv -= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator* (const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv *= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator/ (const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv /= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator*(T s) const
{
  TVector3 rv(*this);

  rv *= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::operator/(T s) const
{
  TVector3 rv(*this);

  rv /= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::getInverted() const
{
  TVector3 rv(*this);

  rv.invert();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::getNormalized() const
{
  TVector3 rv(*this);

  rv.normalize();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::getCrossProduct(const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv.crossProduct(vec);

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T>
TVector3<T>::getReflection(const TVector3 & vec) const
{
  TVector3 rv(*this);

  rv.reflection(vec);

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator+=(const TVector3 & vec)
{
  x += vec.x;
  y += vec.y;
  z += vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator-=(const TVector3 & vec)
{
  x -= vec.x;
  y -= vec.y;
  z -= vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator*=(const TVector3 & vec)
{
  x *= vec.x;
  y *= vec.y;
  z *= vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator/=(const TVector3 & vec)
{
  x /= vec.x;
  y /= vec.y;
  z /= vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator*=(const T s)
{
  x *= s;
  y *= s;
  z *= s;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::operator/=(const T s)
{
  if(s != 0)
  {
    T s2 = 1.0f / s;

    x *= s2;
    y *= s2;
    z *= s2;
  }

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::invert()
{
  x = 0 - x;
  y = 0 - y;
  z = 0 - z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::normalize()
{
  return *this /= length();
}

//---------------------------------------------------------------------------
template <class T>
inline TVector3<T> &
TVector3<T>::crossProduct(const TVector3 & vec)
{
  T tx, ty;

  tx = ((y * vec.z) - (z * vec.y));
  ty = ((z * vec.x) - (x * vec.z));
  z  = ((x * vec.y) - (y * vec.x));
  x  = tx;
  y  = ty;

  return (*this);
}

//---------------------------------------------------------------------------
// Reflect this vector around "vec". "vec" will usually be a normal vector.
template <class T>
inline TVector3<T> &
TVector3<T>::reflection(const TVector3 & vec)
{
  *this = vec * 2 * (vec.dotProduct(*this)) - *this;

  return (*this);
}

//---------------------------------------------------------------------------
// From geon on the devmaster.net forum
template <class T>
inline T
TVector3<T>::length() const
{
  T a, b, c;

  a = x > 0 ? x : 0-x;
  b = y > 0 ? y : 0-y;
  c = z > 0 ? z : 0-z;

  // Assigning the greatest value to a.
  if((b > a) && (b > c))
  {
    T tmp = b;
    b = a;
    a = tmp;
  }
  else if((c > a) && (c > b))
  {
    T tmp = c;
    c = a;
    a = tmp;
  }

  // I found this value optimal. There is probably no point in finetuning it further, since there still is a 20% fault... (For an int version of the same function, use a one step bitshift (a+((b+c)>>1)) instead)
  return a + (b + c) * 0.366;
//  return sqrtf(x*x + y*y + z*z);
}

//---------------------------------------------------------------------------
template <class T>
inline T
TVector3<T>::dotProduct(const TVector3 & vec) const
{
  return (x*vec.x + y*vec.y + z*vec.z);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4(const T * vec)
 : x(vec[0]), y(vec[1]), z(vec[2]), w(vec[3])
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4(const TVector3<T> & vec)
 : x(vec.x), y(vec.y), z(vec.z), w(1)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4(const TVector4 & vec)
 : x(vec.x), y(vec.y), z(vec.z), w(vec.w)
{
}

//---------------------------------------------------------------------------
template <class T>
inline
TVector4<T>::TVector4(T _x, T _y, T _z, T _w)
 : x(_x), y(_y), z(_z), w(_w)
{
}

//---------------------------------------------------------------------------
template <class T>
inline T &
TVector4<T>::operator[](uint8_t item)
{
  static T * vector[4] = {&x, &y, &z, &w};

  return *vector[item];
}

//---------------------------------------------------------------------------
template <class T>
inline const T &
TVector4<T>::operator[](uint8_t item) const
{
  const static T * vector[4] = {&x, &y, &z, &w};

  return *vector[item];
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator= (const T * vec)
{
  x = vec[0];
  y = vec[1];
  z = vec[2];
  w = vec[3];

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator= (const TVector3<T> & vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = 1;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator= (const TVector4 & vec)
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator+(const TVector4 & vec) const
{
  TVector4 rv(*this);

  rv += vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator-(const TVector4 & vec) const
{
  TVector4 rv(*this);

  rv -= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator* (const TVector4 & vec) const
{
  TVector4 rv(*this);

  rv *= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator/ (const TVector4 & vec) const
{
  TVector4 rv(*this);

  rv /= vec;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator*(T s) const
{
  TVector4 rv(*this);

  rv *= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::operator/(T s) const
{
  TVector4 rv(*this);

  rv /= s;

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::getInverted() const
{
  TVector4 rv(*this);

  rv.invert();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::getNormalized() const
{
  TVector4 rv(*this);

  rv.normalize();

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::getCrossProduct(const TVector4 & vec) const
{
  TVector4 rv(*this);

  rv.crossProduct(vec);

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T>
TVector4<T>::getReflection(const TVector4 & vec) const
{
  TVector4 rv(*this);

  rv.reflection(vec);

  return rv;
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator+=(const TVector4 & vec)
{
  x += vec.x;
  y += vec.y;
  z += vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator-=(const TVector4 & vec)
{
  x -= vec.x;
  y -= vec.y;
  z -= vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator*=(const TVector4 & vec)
{
  x *= vec.x;
  y *= vec.y;
  z *= vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator/=(const TVector4 & vec)
{
  x /= vec.x;
  y /= vec.y;
  z /= vec.z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator*=(const T s)
{
  x *= s;
  y *= s;
  z *= s;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::operator/=(const T s)
{
  if(s != 0)
  {
    T s2 = 1.0f / s;

    x *= s2;
    y *= s2;
    z *= s2;
  }

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::invert()
{
  x = 0 - x;
  y = 0 - y;
  z = 0 - z;

  return (*this);
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::normalize()
{
  return *this /= length();
}

//---------------------------------------------------------------------------
template <class T>
inline TVector4<T> &
TVector4<T>::crossProduct(const TVector4 & vec)
{
  T tx, ty;

  tx = ((y * vec.z) - (z * vec.y));
  ty = ((z * vec.x) - (x * vec.z));
  z  = ((x * vec.y) - (y * vec.x));
  x  = tx;
  y  = ty;

  return (*this);
}

//---------------------------------------------------------------------------
// Reflect this vector around "vec". "vec" will usually be a normal vector.
template <class T>
inline TVector4<T> &
TVector4<T>::reflection(const TVector4 & vec)
{
  *this = vec * 2 * (vec.dotProduct(*this)) - *this;

  return (*this);
}

//---------------------------------------------------------------------------
// From geon on the devmaster.net forum
template <class T>
inline T
TVector4<T>::length() const
{
  T a, b, c;

  a = x > 0 ? x : 0-x;
  b = y > 0 ? y : 0-y;
  c = z > 0 ? z : 0-z;

  // Assigning the greatest value to a.
  if((b > a) && (b > c))
  {
    T tmp = b;
    b = a;
    a = tmp;
  }
  else if((c > a) && (c > b))
  {
    T tmp = c;
    c = a;
    a = tmp;
  }

  // I found this value optimal. There is probably no point in finetuning it further, since there still is a 20% fault... (For an int version of the same function, use a one step bitshift (a+((b+c)>>1)) instead)
  return w * (a + (b + c) * 0.366);
//  return w * sqrtf(x*x + y*y + z*z);
}

//---------------------------------------------------------------------------
template <class T>
inline T
TVector4<T>::dotProduct(const TVector4 & vec) const
{
  return (x*vec.x + y*vec.y + z*vec.z);
}
