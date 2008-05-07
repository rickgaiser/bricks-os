#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H


#include "GLES/gl.h"
#include "inttypes.h"


// Conversions
#define fpfromi(c,a)       ((a)<<(c))
#define fpfromf(c,a)       ((GLfixed)((a)*(1<<(c))))
#define fpfromd(c,a)       ((GLfixed)((a)*(1<<(c))))
#define fptoi(c,a)         ((a)>>(c))
#define fptof(c,a)         ((float)(a)/(1<<(c)))
#define fptod(c,a)         ((double)(a)/(1<<(c)))
#define fp32to64(c,a)      (((int64_t)(a))<<(c))
#define fp64to32(c,a)      ((a)>>(c))
#define fp16to32(c,a)      (((int32_t)(a))<<(c))
#define fp32to16(c,a)      ((a)>>(c))

// Math (Use at own risk)
//#define fpmul(c,a,b)       (((a)*(b))>>(c))
#define fpmul(c,a,b)       (((a)>>((c)/2))*((b)>>((c)/2)))
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
#define gl_fpmul(i1,i2) fpmul(FP_PRESICION_GL,(i1),(i2))
#define gl_fpdiv(i1,i2) fpdiv32(FP_PRESICION_GL,(i1),(i2))


struct CFixed
{
#ifdef __cplusplus
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

  operator float()   {return gl_fptof(value);}
  operator double()  {return gl_fptod(value);}
  operator int32_t() {return gl_fptoi(value);}

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

  // Comparison Operators
  bool     operator==(const CFixed & fx) const {return (value == fx.value);}
  bool     operator!=(const CFixed & fx) const {return (value != fx.value);}
  bool     operator> (const CFixed & fx) const {return (value >  fx.value);}
  bool     operator< (const CFixed & fx) const {return (value <  fx.value);}
  bool     operator>=(const CFixed & fx) const {return (value >= fx.value);}
  bool     operator<=(const CFixed & fx) const {return (value <= fx.value);}
#endif // __cplusplus

  volatile GLfixed value;
};


#ifdef __cplusplus
inline CFixed operator+ (float   f, const CFixed & fx){return fx + f;}
inline CFixed operator- (float   f, const CFixed & fx){return fx - f;}
inline CFixed operator* (float   f, const CFixed & fx){return fx * f;}
inline CFixed operator/ (float   f, const CFixed & fx){return fx / f;}
inline CFixed operator+ (double  d, const CFixed & fx){return fx + d;}
inline CFixed operator- (double  d, const CFixed & fx){return fx - d;}
inline CFixed operator* (double  d, const CFixed & fx){return fx * d;}
inline CFixed operator/ (double  d, const CFixed & fx){return fx / d;}
inline CFixed operator+ (int32_t i, const CFixed & fx){return fx + i;}
inline CFixed operator- (int32_t i, const CFixed & fx){return fx - i;}
inline CFixed operator* (int32_t i, const CFixed & fx){return fx * i;}
inline CFixed operator/ (int32_t i, const CFixed & fx){return fx / i;}
#endif // __cplusplus


#endif
