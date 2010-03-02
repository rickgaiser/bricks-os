/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H


#include "inttypes.h"


// Conversions
#define fpfromi(sh,fp)       ((fp)<<(sh))
#define fpfromf(sh,fp)       ((int32_t)((fp)*(1<<(sh))))
#define fpfromd(sh,fp)       ((int32_t)((fp)*(1<<(sh))))
#define fptof(sh,fp)         ((float )(fp)*(1.0/(1<<(sh))))
#define fptod(sh,fp)         ((double)(fp)*(1.0/(1<<(sh))))
//#define fptoi(sh,fp)         ((fp)>>(sh))
#define fpceil(sh,fp)        (((fp)+((1<<sh)-1)) >> sh)
#define fpfloor(sh,fp)       ( (fp)              >> sh)
#define fpround(sh,fp)       (((fp)+(1<<(sh-1))) >> sh)


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
#define gl_fptof(i)     fptof(FP_PRESICION_GL,(i))
#define gl_fptod(i)     fptod(FP_PRESICION_GL,(i))
//#define gl_fptoi(sh,fp) fptoi(FP_PRESICION_GL,(i))
#define gl_fpceil(i)    fpceil(FP_PRESICION_GL,(i))
#define gl_fpfloor(i)   fpfloor(FP_PRESICION_GL,(i))
#define gl_fpround(i)   fpround(FP_PRESICION_GL,(i))
//#define gl_fpmul(i1,i2) fpmul32(FP_PRESICION_GL,(i1),(i2))
#define gl_fpmul(i1,i2) fpmul(FP_PRESICION_GL,(i1),(i2))
#define gl_fpipmul(i1,i2) fpipmul(FP_PRESICION_GL,(i1),(i2))
#define gl_fpipipmul(i1,i2) fpipipmul(FP_PRESICION_GL,(i1),(i2))
#define gl_fpdiv(i1,i2) fpdiv32(FP_PRESICION_GL,(i1),(i2))
#define gl_fpinverse(i) ((0xffffffff) / (i)) // 1<<16<<16 == 0x100000000 ~~ 0xffffffff


#ifdef __cplusplus
template <int p>
class TFixed
{
public:
  // Constructors
  TFixed(){}
  TFixed(const TFixed & fx) : value(fx.value)      {}
  TFixed(float  f)          : value(fpfromf(p, f)) {}
  TFixed(double d)          : value(fpfromd(p, d)) {}
  TFixed(int    i)          : value(fpfromi(p, i)) {}

  // Assignment Operators
  TFixed & operator= (const TFixed & fx){value = fx.value;      return (*this);}
  TFixed & operator= (float  f)         {value = fpfromf(p, f); return (*this);}
  TFixed & operator= (double d)         {value = fpfromd(p, d); return (*this);}
  TFixed & operator= (int    i)         {value = fpfromi(p, i); return (*this);}

  operator float()   const {return fptof  (p, value);}
  operator double()  const {return fptod  (p, value);}
  operator int()     const {return fpfloor(p, value);}

  // Compound Assignment Operators
  TFixed & operator+=(const TFixed & fx){value += fx.value;                       return (*this);}
  TFixed & operator-=(const TFixed & fx){value -= fx.value;                       return (*this);}
  TFixed & operator*=(const TFixed & fx){value  = fpmul(p, value, fx.value);      return (*this);}
  TFixed & operator/=(const TFixed & fx){value  = fpdiv(p, value, fx.value);      return (*this);}

  TFixed & operator+=(float  f)         {value += fpfromf(p, f);                  return (*this);}
  TFixed & operator-=(float  f)         {value -= fpfromf(p, f);                  return (*this);}
  TFixed & operator*=(float  f)         {value  = fpmul(p, value, fpfromf(p, f)); return (*this);}
  TFixed & operator/=(float  f)         {value  = fpdiv(p, value, fpfromf(p, f)); return (*this);}

  TFixed & operator+=(double d)         {value += fpfromd(p, d);                  return (*this);}
  TFixed & operator-=(double d)         {value -= fpfromd(p, d);                  return (*this);}
  TFixed & operator*=(double d)         {value  = fpmul(p, value, fpfromd(p, d)); return (*this);}
  TFixed & operator/=(double d)         {value  = fpdiv(p, value, fpfromd(p, d)); return (*this);}

  TFixed & operator+=(int    i)         {value += fpfromi(p, i);                  return (*this);}
  TFixed & operator-=(int    i)         {value -= fpfromi(p, i);                  return (*this);}
  TFixed & operator*=(int    i)         {value *= i;                              return (*this);}
  TFixed & operator/=(int    i)         {value /= i;                              return (*this);}

  // Binary Arithmetic Operators
  TFixed   operator+ (const TFixed & fx) const {TFixed rv(*this); rv += fx; return rv;}
  TFixed   operator- (const TFixed & fx) const {TFixed rv(*this); rv -= fx; return rv;}
  TFixed   operator* (const TFixed & fx) const {TFixed rv(*this); rv *= fx; return rv;}
  TFixed   operator/ (const TFixed & fx) const {TFixed rv(*this); rv /= fx; return rv;}

  TFixed   operator+ (float  f)          const {TFixed rv(*this); rv += f;  return rv;}
  TFixed   operator- (float  f)          const {TFixed rv(*this); rv -= f;  return rv;}
  TFixed   operator* (float  f)          const {TFixed rv(*this); rv *= f;  return rv;}
  TFixed   operator/ (float  f)          const {TFixed rv(*this); rv /= f;  return rv;}

  TFixed   operator+ (double d)          const {TFixed rv(*this); rv += d;  return rv;}
  TFixed   operator- (double d)          const {TFixed rv(*this); rv -= d;  return rv;}
  TFixed   operator* (double d)          const {TFixed rv(*this); rv *= d;  return rv;}
  TFixed   operator/ (double d)          const {TFixed rv(*this); rv /= d;  return rv;}

  TFixed   operator+ (int    i)          const {TFixed rv(*this); rv += i;  return rv;}
  TFixed   operator- (int    i)          const {TFixed rv(*this); rv -= i;  return rv;}
  TFixed   operator* (int    i)          const {TFixed rv(*this); rv *= i;  return rv;}
  TFixed   operator/ (int    i)          const {TFixed rv(*this); rv /= i;  return rv;}

  // Comparison Operators
  bool     operator==(const TFixed & fx) const {return (value == fx.value);}
  bool     operator!=(const TFixed & fx) const {return (value != fx.value);}
  bool     operator> (const TFixed & fx) const {return (value >  fx.value);}
  bool     operator< (const TFixed & fx) const {return (value <  fx.value);}
  bool     operator>=(const TFixed & fx) const {return (value >= fx.value);}
  bool     operator<=(const TFixed & fx) const {return (value <= fx.value);}

  bool     operator==(float  f)          const {return (value == fpfromf(p, f));}
  bool     operator!=(float  f)          const {return (value != fpfromf(p, f));}
  bool     operator> (float  f)          const {return (value >  fpfromf(p, f));}
  bool     operator< (float  f)          const {return (value <  fpfromf(p, f));}
  bool     operator>=(float  f)          const {return (value >= fpfromf(p, f));}
  bool     operator<=(float  f)          const {return (value <= fpfromf(p, f));}

  bool     operator==(double d)          const {return (value == fpfromd(p, d));}
  bool     operator!=(double d)          const {return (value != fpfromd(p, d));}
  bool     operator> (double d)          const {return (value >  fpfromd(p, d));}
  bool     operator< (double d)          const {return (value <  fpfromd(p, d));}
  bool     operator>=(double d)          const {return (value >= fpfromd(p, d));}
  bool     operator<=(double d)          const {return (value <= fpfromd(p, d));}

  bool     operator==(int    i)          const {return (value == fpfromi(p, i));}
  bool     operator!=(int    i)          const {return (value != fpfromi(p, i));}
  bool     operator> (int    i)          const {return (value >  fpfromi(p, i));}
  bool     operator< (int    i)          const {return (value <  fpfromi(p, i));}
  bool     operator>=(int    i)          const {return (value >= fpfromi(p, i));}
  bool     operator<=(int    i)          const {return (value <= fpfromi(p, i));}

  int32_t value;
};

// float
template <int p> inline float  &  operator+=(float  & f, const TFixed<p> & fx){return (f += (float)fx);}
template <int p> inline float  &  operator-=(float  & f, const TFixed<p> & fx){return (f -= (float)fx);}
template <int p> inline float  &  operator*=(float  & f, const TFixed<p> & fx){return (f  = (float)fx);}
template <int p> inline float  &  operator/=(float  & f, const TFixed<p> & fx){return (f  = (float)fx);}
// double
template <int p> inline double &  operator+=(double & d, const TFixed<p> & fx){return (d += (double)fx);}
template <int p> inline double &  operator-=(double & d, const TFixed<p> & fx){return (d -= (double)fx);}
template <int p> inline double &  operator*=(double & d, const TFixed<p> & fx){return (d *= (double)fx);}
template <int p> inline double &  operator/=(double & d, const TFixed<p> & fx){return (d /= (double)fx);}
// int
template <int p> inline int    &  operator+=(int    & i, const TFixed<p> & fx){return (i += (int)fx);}
template <int p> inline int    &  operator-=(int    & i, const TFixed<p> & fx){return (i -= (int)fx);}
template <int p> inline int    &  operator*=(int    & i, const TFixed<p> & fx){return (i  = (int)(TFixed<p>(i) * fx));} // NOTE: converting i for more accuracy, but this limits the range
template <int p> inline int    &  operator/=(int    & i, const TFixed<p> & fx){return (i  = (int)(TFixed<p>(i) / fx));} // NOTE: converting i for more accuracy, but this limits the range

// float
template <int p> inline TFixed<p> operator+ (float    f, const TFixed<p> & fx){return TFixed<p>(f) + fx;}
template <int p> inline TFixed<p> operator- (float    f, const TFixed<p> & fx){return TFixed<p>(f) - fx;}
template <int p> inline TFixed<p> operator* (float    f, const TFixed<p> & fx){return TFixed<p>(f) * fx;}
template <int p> inline TFixed<p> operator/ (float    f, const TFixed<p> & fx){return TFixed<p>(f) / fx;}
// double
template <int p> inline TFixed<p> operator+ (double   d, const TFixed<p> & fx){return TFixed<p>(d) + fx;}
template <int p> inline TFixed<p> operator- (double   d, const TFixed<p> & fx){return TFixed<p>(d) - fx;}
template <int p> inline TFixed<p> operator* (double   d, const TFixed<p> & fx){return TFixed<p>(d) * fx;}
template <int p> inline TFixed<p> operator/ (double   d, const TFixed<p> & fx){return TFixed<p>(d) / fx;}
// int
template <int p> inline TFixed<p> operator+ (int      i, const TFixed<p> & fx){return TFixed<p>(i) + fx;}
template <int p> inline TFixed<p> operator- (int      i, const TFixed<p> & fx){return TFixed<p>(i) - fx;}
template <int p> inline TFixed<p> operator* (int      i, const TFixed<p> & fx){return TFixed<p>(i) * fx;}
template <int p> inline TFixed<p> operator/ (int      i, const TFixed<p> & fx){return TFixed<p>(i) / fx;}

// float
template <int p> inline bool      operator==(float    f, const TFixed<p> & fx){return (f == (float)fx);}
template <int p> inline bool      operator!=(float    f, const TFixed<p> & fx){return (f != (float)fx);}
template <int p> inline bool      operator> (float    f, const TFixed<p> & fx){return (f >  (float)fx);}
template <int p> inline bool      operator< (float    f, const TFixed<p> & fx){return (f <  (float)fx);}
template <int p> inline bool      operator>=(float    f, const TFixed<p> & fx){return (f >= (float)fx);}
template <int p> inline bool      operator<=(float    f, const TFixed<p> & fx){return (f <= (float)fx);}
// double
template <int p> inline bool      operator==(double   d, const TFixed<p> & fx){return (d == (double)fx);}
template <int p> inline bool      operator!=(double   d, const TFixed<p> & fx){return (d != (double)fx);}
template <int p> inline bool      operator> (double   d, const TFixed<p> & fx){return (d >  (double)fx);}
template <int p> inline bool      operator< (double   d, const TFixed<p> & fx){return (d <  (double)fx);}
template <int p> inline bool      operator>=(double   d, const TFixed<p> & fx){return (d >= (double)fx);}
template <int p> inline bool      operator<=(double   d, const TFixed<p> & fx){return (d <= (double)fx);}
// int
template <int p> inline bool      operator==(int      i, const TFixed<p> & fx){return (i == (int)fx);}
template <int p> inline bool      operator!=(int      i, const TFixed<p> & fx){return (i != (int)fx);}
template <int p> inline bool      operator> (int      i, const TFixed<p> & fx){return (i >  (int)fx);}
template <int p> inline bool      operator< (int      i, const TFixed<p> & fx){return (i <  (int)fx);}
template <int p> inline bool      operator>=(int      i, const TFixed<p> & fx){return (i >= (int)fx);}
template <int p> inline bool      operator<=(int      i, const TFixed<p> & fx){return (i <= (int)fx);}


typedef TFixed<16> CFixed;


#endif // __cplusplus


#endif
