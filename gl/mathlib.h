#ifndef MATHLIB_H
#define MATHLIB_H


#include "stdint.h"


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

//-----------------------------------------------------------------------------
inline unsigned int
fast_int_log2(int x)
{
  unsigned int rv = 0;

  while(x > (1 << rv))
    rv++;

  return rv;
}

//-----------------------------------------------------------------------------
union float_t
{
  float    f;
  int32_t  i;
  uint32_t ui;

  struct
  {
    uint32_t mantissa : 23;
    uint32_t exponent :  8;
    uint32_t sign     :  1;
  };
};

//-----------------------------------------------------------------------------
#define LOG2_TABLE_SIZE_LOG2 6
#define LOG2_TABLE_SIZE      (1 << LOG2_TABLE_SIZE_LOG2)
extern const float log2Table[LOG2_TABLE_SIZE];
//-----------------------------------------------------------------------------
inline float
fast_log2(float x)
{
  float rv;
  float_t fl;

  fl.f = x;

  // Extract the exponent from the floating point value
  rv  = (fl.exponent - 127);
  // Add mantissa part from lookup table
  rv += log2Table[fl.mantissa >> (23 - LOG2_TABLE_SIZE_LOG2)];

  return rv ;
}

//-----------------------------------------------------------------------------
#define EXP2_TABLE_SIZE_LOG2 6
#define EXP2_TABLE_SIZE      (1 << EXP2_TABLE_SIZE_LOG2)
#define EXP2_TABLE_OFFSET    (EXP2_TABLE_SIZE/2)
#define EXP2_TABLE_SCALE     ((float)(EXP2_TABLE_SIZE/2))
extern const float exp2Table[EXP2_TABLE_SIZE];
//-----------------------------------------------------------------------------
/*
 * Fast version of 2^x
 * Identity: exp2(a + b) = exp2(a) * exp2(b)
 * Let ipart = int(x)
 * Let fpart = x - ipart;
 * So, exp2(x) = exp2(ipart) * exp2(fpart)
 * Compute exp2(ipart) with i << ipart
 * Compute exp2(fpart) with lookup table.
 */
inline float
fast_exp2(float x)
{
   int32_t ipart;
   float fpart, mpart;
   union float_t epart;

   if(x > 129.00000f)
      return 3.402823466e+38f;

   if(x < -126.99999f)
      return 0.0f;

   ipart = (int32_t)x;
   fpart = x - (float)ipart;

   epart.exponent = (ipart + 127);

   mpart = exp2Table[EXP2_TABLE_OFFSET + (int)(fpart * EXP2_TABLE_SCALE)];

   return epart.f * mpart;
}

//-----------------------------------------------------------------------------
// Fast approximation to exp(x).
inline float
fast_exp(float x)
{
   const float k = 1.44269f; /* = log2(e) */
   return fast_exp2(k * x);
}

//-----------------------------------------------------------------------------
inline float
fast_pow(float x, float y)
{
   return fast_exp2(fast_log2(x) * y);
}


}


#endif
