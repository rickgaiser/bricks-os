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
inline T abs(const T & a)
{
  return a > 0 ? a : -a;
}

//-----------------------------------------------------------------------------
template<class T>
inline T min(const T & a, const T & b)
{
  return a < b ? a : b;
}

//-----------------------------------------------------------------------------
template<class T>
inline T max(const T & a, const T & b)
{
  return a > b ? a : b;
}

//-----------------------------------------------------------------------------
template<class T>
inline T min(const T & a, const T & b, const T & c)
{
  return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

//-----------------------------------------------------------------------------
template<class T>
inline T max(const T & a, const T & b, const T & c)
{
  return a > b ? (a > c ? a : c) : (b > c ? b : c);
}

//-----------------------------------------------------------------------------
template<class T>
inline T clamp(const T & a, const T & min, const T & max)
{
  return a < min ? min : (a > max ? max : a);
}

//-----------------------------------------------------------------------------
template<class T>
inline T clamp_min(const T & a, const T & min)
{
  return a < min ? min : a;
}

//-----------------------------------------------------------------------------
template<class T>
inline T clamp_max(const T & a, const T & max)
{
  return a > max ? max : a;
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
template<class T, class F>
inline T lerp(const F & a, const T & v0, const T & v1)
{
  return v0 + (v1 - v0) * a;
}

//-----------------------------------------------------------------------------
template<class T, class F>
inline T lerp_fx(const F & a, const T & v0, const T & v1, const unsigned int sh)
{
  return v0 + (((v1 - v0) * a) >> sh);
}

//-----------------------------------------------------------------------------
template<class T>
inline T lerp_2d(const T & a, const T & b,
  const T & v00, const T & v10, const T & v01, const T & v11)
{
   const T temp0 = lerp(a, v00, v10);
   const T temp1 = lerp(a, v01, v11);
   return lerp(b, temp0, temp1);
}

//-----------------------------------------------------------------------------
template<class T>
inline T lerp_3d(const T & a, const T & b, const T & c,
  const T & v000, const T & v100, const T & v010, const T & v110,
  const T & v001, const T & v101, const T & v011, const T & v111)
{
   const T temp0 = lerp_2d(a, b, v000, v100, v010, v110);
   const T temp1 = lerp_2d(a, b, v001, v101, v011, v111);
   return lerp(c, temp0, temp1);
}

//-----------------------------------------------------------------------------
template<class T>
inline T fast_int_pow(const T & x, unsigned int y)
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
