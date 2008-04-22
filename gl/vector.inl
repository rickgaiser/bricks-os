#include "vector.h"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline
CVectorF::CVectorF(const GLfloat * vec)
 : x(vec[0]), y(vec[1]), z(vec[2]), w(vec[3])
{
}

//---------------------------------------------------------------------------
inline
CVectorF::CVectorF(const CVectorF & vec)
 : x(vec.x), y(vec.y), z(vec.z), w(vec.w)
{
}

//---------------------------------------------------------------------------
inline
CVectorF::CVectorF(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _w)
 : x(_x), y(_y), z(_z), w(_w)
{
}

//---------------------------------------------------------------------------
inline CVectorF
CVectorF::operator+(const CVectorF & vec)
{
  CVectorF rv(this);

  rv += vec;

  return rv;
}

//---------------------------------------------------------------------------
inline CVectorF
CVectorF::operator-(const CVectorF & vec)
{
  CVectorF rv(this);

  rv -= vec;

  return rv;
}

//---------------------------------------------------------------------------
inline CVectorF
CVectorF::operator*(GLfloat s)
{
  CVectorF rv(this);

  rv *= s;

  return rv;
}

//---------------------------------------------------------------------------
inline CVectorF
CVectorF::operator/(GLfloat s)
{
  CVectorF rv(this);

  rv /= s;

  return rv;
}

//---------------------------------------------------------------------------
inline CVectorF
CVectorF::getInverted()
{
  CVectorF rv(this);

  rv.invert();

  return rv;
}

//---------------------------------------------------------------------------
inline CVectorF
CVectorF::getNormalized()
{
  CVectorF rv(this);

  rv.normalize();

  return rv;
}

//---------------------------------------------------------------------------
inline CVectorF
CVectorF::getCrossProduct(const CVectorF & vec)
{
  CVectorF rv(this);

  rv.reflection();

  return rv;
}

//---------------------------------------------------------------------------
inline CVectorF &
CVectorF::operator+=(const CVectorF & vec)
{
  x += vec.x;
  y += vec.y;
  z += vec.z;
  w += vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
inline CVectorF &
CVectorF::operator-=(const CVectorF & vec);
{
  x -= vec.x;
  y -= vec.y;
  z -= vec.z;
  w -= vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
inline CVectorF &
CVectorF::operator*=(const GLfloat s);
{
  x *= s;
  y *= s;
  z *= s;
  w *= s;

  return (*this);
}

//---------------------------------------------------------------------------
inline CVectorF &
CVectorF::operator/=(const GLfloat s);
{
  if(s != 0)
  {
    GLfloat s2 = 1.0f / s;

    x *= s2;
    y *= s2;
    z *= s2;
    w *= s2;
  }

  return (*this);
}

//---------------------------------------------------------------------------
inline CVectorF &
CVectorF::invert()
{
  x = -x;
  y = -y;
  z = -z;
  w = -w;

  return (*this);
}

//---------------------------------------------------------------------------
inline CVectorF &
CVectorF::normalize()
{
  return *this /= length();
}

//---------------------------------------------------------------------------
inline CVectorF &
CVectorF::crossProduct(const CVectorF & vec)
{
  return (*this - ((2.0f * vec.innerProduct(*this)) * vec));
}

//---------------------------------------------------------------------------
inline CVectorF &
CVectorF::operator= (const CVectorF & vec);
{
  x = vec.x;
  y = vec.y;
  z = vec.z;
  w = vec.w;

  return (*this);
}

//---------------------------------------------------------------------------
inline GLfloat
CVectorF::length()
{
  return sqrt(x*x + y*y + z*z);
}

//---------------------------------------------------------------------------
inline GLfloat
CVectorF::dotProduct(const CVectorF & vec)
{
  return (x*vec.x + y*vec.y + z*vec.z);
}
