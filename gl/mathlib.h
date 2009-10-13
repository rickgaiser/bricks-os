#ifndef MATHLIB_H
#define MATHLIB_H


#include "inttypes.h"


#undef abs
#undef min
#undef max


namespace mathlib
{


//-----------------------------------------------------------------------------
template<class T>
inline T abs(T a)
{
  return a > 0 ? a : -a;
}

//-----------------------------------------------------------------------------
template<class T>
inline T min(T a, T b)
{
  return a < b ? a : b;
}

//-----------------------------------------------------------------------------
template<class T>
inline T max(T a, T b)
{
  return a > b ? a : b;
}

//-----------------------------------------------------------------------------
template<class T>
inline T min(T a, T b, T c)
{
  return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

//-----------------------------------------------------------------------------
template<class T>
inline T max(T a, T b, T c)
{
  return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

//-----------------------------------------------------------------------------
template<class T>
inline T clamp(T a, T min, T max)
{
  return a < min ? min : (a > max ? max : a);
}

//-----------------------------------------------------------------------------
template<class T>
inline void swap(T & a, T & b)
{
  T tmp = a;
  a = b;
  b = tmp;
}

//-----------------------------------------------------------------------------
// NOTE: Use the template functions below if possible
#define mathlib_LERP(a,v0,v1) ((v0) + ((v1) - (v0)) * (a))

//-----------------------------------------------------------------------------
template<class T>
inline T lerp(T a, T v0, T v1)
{
  return v0 + a * (v1 - v0);
}

//-----------------------------------------------------------------------------
template<class T>
inline T lerp_2d(T a, T b,
  T v00, T v10, T v01, T v11)
{
   const T temp0 = lerp(a, v00, v10);
   const T temp1 = lerp(a, v01, v11);
   return lerp(b, temp0, temp1);
}

//-----------------------------------------------------------------------------
template<class T>
inline T lerp_3d(T a, T b, T c,
  T v000, T v100, T v010, T v110,
  T v001, T v101, T v011, T v111)
{
   const T temp0 = lerp_2d(a, b, v000, v100, v010, v110);
   const T temp1 = lerp_2d(a, b, v001, v101, v011, v111);
   return lerp(c, temp0, temp1);
}

//-----------------------------------------------------------------------------
template<class T>
inline T fast_int_pow(T x, unsigned int y)
{
  T rv(1);
  for(unsigned int i = 0; i < y; i++)
    rv *= x;
  return rv;
}


}


#endif
