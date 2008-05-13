#ifndef GL_COLOR_H
#define GL_COLOR_H


//---------------------------------------------------------------------------
template <class T>
class TColor
{
public:
  TColor(){}
  TColor(const TColor & c);
  TColor(const T * c);
  TColor(T _r, T _g, T _b, T _a);

  TColor & operator= (const TColor & c);
  TColor & operator= (const T * c);

  TColor   operator+ (const TColor & c) const;
  TColor   operator- (const TColor & c) const;
  TColor   operator* (const TColor & c) const;
  TColor   operator/ (const TColor & c) const;
  TColor   operator+ (T s) const;
  TColor   operator- (T s) const;
  TColor   operator* (T s) const;
  TColor   operator/ (T s) const;
  TColor   getClamped() const;

  TColor & operator+=(const TColor & c);
  TColor & operator-=(const TColor & c);
  TColor & operator*=(const TColor & c);
  TColor & operator/=(const TColor & c);
  TColor & operator+=(T s);
  TColor & operator-=(T s);
  TColor & operator*=(T s);
  TColor & operator/=(T s);
  TColor & clamp();

public:
  T r;
  T g;
  T b;
  T a;
};


#include "color.inl"


#endif // GL_VECTOR_H
