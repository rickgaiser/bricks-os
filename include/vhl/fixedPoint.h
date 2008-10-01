#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H


#include "inttypes.h"


// Conversions
#define fpfromi(c,a)       ((a)<<(c))
#define fpfromf(c,a)       ((int32_t)((a)*(1<<(c))))
#define fpfromd(c,a)       ((int32_t)((a)*(1<<(c))))
#define fptoi(c,a)         ((a)>>(c))
#define fptof(c,a)         ((float)(a)/(1<<(c)))
#define fptod(c,a)         ((double)(a)/(1<<(c)))

// Math (Use at own risk)
//#define fpmul(c,a,b)       (((a)*(b))>>(c))
#define fpmul(c,a,b)       (( ((a)>>((c)/2)) * ((b)>>((c)/2)))             ) // normal multiplication (loses half of the precision on both values)
#define fpipmul(c,a,b)     (( ((a)>>((c)/2)) * ((b)>>1      ))>>(((c)/2)-1)) // intepolated mul (loses half of the precision on ONLY FIRST value, and 1 bit of precision on the second)
#define fpipipmul(c,a,b)   (( ((a)>>1      ) * ((b)>>1      ))>>(((c)  )-2)) // 2 interpolation values mul (loses 1 bit of precision on both values)
#define fpdiv(c,a,b)       (((a)<<(c))/(b))
// Math (16 bit)
#define fpmul16(c,a,b)     ((((int32_t)(a))*((int32_t)(b)))>>(c))
#define fpdiv16(c,a,b)     ((((int32_t)(a))<<(c))/(b))
#define fpmul16to32(c,a,b) ((((int32_t)(a))*((int32_t)(b))))
// Math (32 bit)
#define fpmul32(c,a,b)     ((((int64_t)(a))*((int64_t)(b)))>>(c))
#define fpdiv32(c,a,b)     ((((int64_t)(a))<<(c))/(b))
#define fpmul32to64(c,a,b) (((int64_t)(a))*((int64_t)(b)))

// Macro's for OpenGL (16.16)
#define FP_PRESICION_GL 16
#define gl_fpfromi(i)   fpfromi(FP_PRESICION_GL,(i))
#define gl_fpfromf(i)   fpfromf(FP_PRESICION_GL,(i))
#define gl_fpfromd(i)   fpfromd(FP_PRESICION_GL,(i))
#define gl_fptoi(i)     fptoi(FP_PRESICION_GL,(i))
#define gl_fptof(i)     fptof(FP_PRESICION_GL,(i))
#define gl_fptod(i)     fptod(FP_PRESICION_GL,(i))
//#define gl_fpmul(i1,i2) fpmul32(FP_PRESICION_GL,(i1),(i2))
#define gl_fpmul(i1,i2) fpmul(FP_PRESICION_GL,(i1),(i2))
#define gl_fpipmul(i1,i2) fpipmul(FP_PRESICION_GL,(i1),(i2))
#define gl_fpipipmul(i1,i2) fpipipmul(FP_PRESICION_GL,(i1),(i2))
#define gl_fpdiv(i1,i2) fpdiv32(FP_PRESICION_GL,(i1),(i2))
#define gl_fpinverse(i) ((0xffffffff) / (i)) // 1<<16<<16 == 0x100000000 ~~ 0xffffffff


#ifdef __cplusplus
class CFixed
{
public:
  // Constructors
  CFixed(){}
  CFixed(const CFixed & fx) : value(fx.value)      {}
  CFixed(float  f)          : value(gl_fpfromf(f)) {}
  CFixed(double d)          : value(gl_fpfromd(d)) {}
  CFixed(int32_t i)         : value(gl_fpfromi(i)) {}

  // Assignment Operators
  CFixed & operator= (const CFixed & fx){value = fx.value;      return (*this);}
  CFixed & operator= (float  f)         {value = gl_fpfromf(f); return (*this);}
  CFixed & operator= (double d)         {value = gl_fpfromd(d); return (*this);}
  CFixed & operator= (int32_t i)        {value = gl_fpfromi(i); return (*this);}

  operator float()   const {return gl_fptof(value);}
  operator double()  const {return gl_fptod(value);}
  operator int32_t() const {return gl_fptoi(value);}

  // Compound Assignment Operators
  CFixed & operator+=(const CFixed & fx){value += fx.value;                      return (*this);}
  CFixed & operator-=(const CFixed & fx){value -= fx.value;                      return (*this);}
  CFixed & operator*=(const CFixed & fx){value = gl_fpmul(value, fx.value);      return (*this);}
  CFixed & operator/=(const CFixed & fx){value = gl_fpdiv(value, fx.value);      return (*this);}
  CFixed & operator+=(float  f)         {value += gl_fpfromf(f);                 return (*this);}
  CFixed & operator-=(float  f)         {value -= gl_fpfromf(f);                 return (*this);}
  CFixed & operator*=(float  f)         {value = gl_fpmul(value, gl_fpfromf(f)); return (*this);}
  CFixed & operator/=(float  f)         {value = gl_fpdiv(value, gl_fpfromf(f)); return (*this);}
  CFixed & operator+=(double d)         {value += gl_fpfromd(d);                 return (*this);}
  CFixed & operator-=(double d)         {value -= gl_fpfromd(d);                 return (*this);}
  CFixed & operator*=(double d)         {value = gl_fpmul(value, gl_fpfromd(d)); return (*this);}
  CFixed & operator/=(double d)         {value = gl_fpdiv(value, gl_fpfromd(d)); return (*this);}
  CFixed & operator+=(int32_t i)        {value += gl_fpfromi(i);                 return (*this);}
  CFixed & operator-=(int32_t i)        {value -= gl_fpfromi(i);                 return (*this);}
  CFixed & operator*=(int32_t i)        {value *= i;                             return (*this);}
  CFixed & operator/=(int32_t i)        {value /= i;                             return (*this);}
  CFixed & ipMulIs   (const CFixed & fx){value = gl_fpipmul(value, fx.value);    return (*this);}
  CFixed & ipipMulIs (const CFixed & fx){value = gl_fpipipmul(value, fx.value);  return (*this);}

  // Binary Arithmetic Operators
  CFixed   operator+ (const CFixed & fx) const {CFixed rv(*this); rv += fx; return rv;}
  CFixed   operator- (const CFixed & fx) const {CFixed rv(*this); rv -= fx; return rv;}
  CFixed   operator* (const CFixed & fx) const {CFixed rv(*this); rv *= fx; return rv;}
  CFixed   operator/ (const CFixed & fx) const {CFixed rv(*this); rv /= fx; return rv;}
  CFixed   operator+ (float  f)          const {CFixed rv(*this); rv += f;  return rv;}
  CFixed   operator- (float  f)          const {CFixed rv(*this); rv -= f;  return rv;}
  CFixed   operator* (float  f)          const {CFixed rv(*this); rv *= f;  return rv;}
  CFixed   operator/ (float  f)          const {CFixed rv(*this); rv /= f;  return rv;}
  CFixed   operator+ (double d)          const {CFixed rv(*this); rv += d;  return rv;}
  CFixed   operator- (double d)          const {CFixed rv(*this); rv -= d;  return rv;}
  CFixed   operator* (double d)          const {CFixed rv(*this); rv *= d;  return rv;}
  CFixed   operator/ (double d)          const {CFixed rv(*this); rv /= d;  return rv;}
  CFixed   operator+ (int32_t i)         const {CFixed rv(*this); rv += i;  return rv;}
  CFixed   operator- (int32_t i)         const {CFixed rv(*this); rv -= i;  return rv;}
  CFixed   operator* (int32_t i)         const {CFixed rv(*this); rv *= i;  return rv;}
  CFixed   operator/ (int32_t i)         const {CFixed rv(*this); rv /= i;  return rv;}
  CFixed   ipMul     (const CFixed & fx) const {CFixed rv(*this); rv.ipMulIs(fx); return rv;}
  CFixed   ipipMul   (const CFixed & fx) const {CFixed rv(*this); rv.ipipMulIs(fx); return rv;}

  // Comparison Operators
  bool     operator==(const CFixed & fx) const {return (value == fx.value);}
  bool     operator!=(const CFixed & fx) const {return (value != fx.value);}
  bool     operator> (const CFixed & fx) const {return (value >  fx.value);}
  bool     operator< (const CFixed & fx) const {return (value <  fx.value);}
  bool     operator>=(const CFixed & fx) const {return (value >= fx.value);}
  bool     operator<=(const CFixed & fx) const {return (value <= fx.value);}
  bool     operator==(float  f)          const {return (value == gl_fpfromf(f));}
  bool     operator!=(float  f)          const {return (value != gl_fpfromf(f));}
  bool     operator> (float  f)          const {return (value >  gl_fpfromf(f));}
  bool     operator< (float  f)          const {return (value <  gl_fpfromf(f));}
  bool     operator>=(float  f)          const {return (value >= gl_fpfromf(f));}
  bool     operator<=(float  f)          const {return (value <= gl_fpfromf(f));}
  bool     operator==(double d)          const {return (value == gl_fpfromd(d));}
  bool     operator!=(double d)          const {return (value != gl_fpfromd(d));}
  bool     operator> (double d)          const {return (value >  gl_fpfromd(d));}
  bool     operator< (double d)          const {return (value <  gl_fpfromd(d));}
  bool     operator>=(double d)          const {return (value >= gl_fpfromd(d));}
  bool     operator<=(double d)          const {return (value <= gl_fpfromd(d));}
  bool     operator==(int32_t i)         const {return (value == gl_fpfromi(i));}
  bool     operator!=(int32_t i)         const {return (value != gl_fpfromi(i));}
  bool     operator> (int32_t i)         const {return (value >  gl_fpfromi(i));}
  bool     operator< (int32_t i)         const {return (value <  gl_fpfromi(i));}
  bool     operator>=(int32_t i)         const {return (value >= gl_fpfromi(i));}
  bool     operator<=(int32_t i)         const {return (value <= gl_fpfromi(i));}

  int32_t value;
};

inline CFixed operator+ (float   f, const CFixed & fx){return CFixed(f) + fx;}
inline CFixed operator- (float   f, const CFixed & fx){return CFixed(f) - fx;}
inline CFixed operator* (float   f, const CFixed & fx){return CFixed(f) * fx;}
inline CFixed operator/ (float   f, const CFixed & fx){return CFixed(f) / fx;}
inline CFixed operator+ (double  d, const CFixed & fx){return CFixed(d) + fx;}
inline CFixed operator- (double  d, const CFixed & fx){return CFixed(d) - fx;}
inline CFixed operator* (double  d, const CFixed & fx){return CFixed(d) * fx;}
inline CFixed operator/ (double  d, const CFixed & fx){return CFixed(d) / fx;}
inline CFixed operator+ (int32_t i, const CFixed & fx){return CFixed(i) + fx;}
inline CFixed operator- (int32_t i, const CFixed & fx){return CFixed(i) - fx;}
inline CFixed operator* (int32_t i, const CFixed & fx){return CFixed(i) * fx;}
inline CFixed operator/ (int32_t i, const CFixed & fx){return CFixed(i) / fx;}
#endif // __cplusplus


#endif
