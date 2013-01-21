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


#include "stdint.h"
#ifdef NDS9
#include "../../kernel/arch/arm/gbands/macros.h"
#endif


// Macro's for 32bit fixed points
// Conversions
#define fpfromi(sh,fp) ((fp)<<(sh))
inline int32_t fpfromf(unsigned int sh, const float   & f ) { return (int32_t)(f  * (1 << sh)); }
inline int32_t fpfromd(unsigned int sh, const double  & d ) { return (int32_t)(d  * (1 << sh)); }
inline float   fptof  (unsigned int sh, const int32_t & fp) { return (float  )(fp * (1.0 / (1 << sh))); }
inline double  fptod  (unsigned int sh, const int32_t & fp) { return (double )(fp * (1.0 / (1 << sh))); }
inline int32_t fpceil (unsigned int sh, const int32_t & fp) { return (fp + ((1 << sh) - 1)) >> sh; }
inline int32_t fpfloor(unsigned int sh, const int32_t & fp) { return  fp                    >> sh; }
inline int32_t fpround(unsigned int sh, const int32_t & fp) { return (fp + (1 << (sh - 1))) >> sh; }
// Math
#define fpmul(sh,a,b)        ((((int64_t)(a))*((int64_t)(b)))>>(sh))
#define fpmul_fast(sh,a,b)   ((((a)>>((sh)>>1)) * ((b)>>((sh)>>1)))) // loses half of the precision on both values
#ifndef NDS9
#define fpdiv(sh,a,b)        ((((int64_t)(a))<<(sh))/(b))
#else
#define fpdiv(sh,a,b)        nds_div_64_32(((int64_t)(a))<<(sh), b)
#endif

// Macro's for OpenGL (16.16)
#define FP_PRESICION_GL 16
// Conversions
#define gl_fpfromi(i) fpfromi(FP_PRESICION_GL,(i))
inline int32_t gl_fpfromf(const float   & f ) { return fpfromf(FP_PRESICION_GL,(f )); }
inline int32_t gl_fpfromd(const double  & d ) { return fpfromd(FP_PRESICION_GL,(d )); }
inline float   gl_fptof  (const int32_t & fp) { return fptof  (FP_PRESICION_GL,(fp)); }
inline double  gl_fptod  (const int32_t & fp) { return fptod  (FP_PRESICION_GL,(fp)); }
inline int32_t gl_fpceil (const int32_t & fp) { return fpceil (FP_PRESICION_GL,(fp)); }
inline int32_t gl_fpfloor(const int32_t & fp) { return fpfloor(FP_PRESICION_GL,(fp)); }
inline int32_t gl_fpround(const int32_t & fp) { return fpround(FP_PRESICION_GL,(fp)); }
// Math
inline int32_t gl_fpmul  (const int32_t & fp1, const int32_t & fp2) { return fpmul(FP_PRESICION_GL,(fp1),(fp2)); }
inline int32_t gl_fpdiv  (const int32_t & fp1, const int32_t & fp2) { return fpdiv(FP_PRESICION_GL,(fp1),(fp2)); }
#ifndef NDS9
#define gl_fpinverse(i)      (0xffffffff / (i)) // 1<<16<<16 == 0x100000000 ~~ 0xffffffff
#else
#define gl_fpinverse(i)      nds_div_32_32(0xffffffff, i)
#endif


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

#ifdef NDS9
//-----------------------------------------------------------------------------
template <int p>
inline TFixed<p>
sqrt(const TFixed<p> & fix)
{
  TFixed<p> retval;
  retval.value = nds_sqrt_64((uint64_t)fix.value << p);
  return retval;
}

//-----------------------------------------------------------------------------
template <int p>
inline TFixed<p>
sqrtf(const TFixed<p> & fix)
{
  return sqrt(fix);
}
#endif


typedef TFixed<16> CFixed;


#endif // __cplusplus


#endif
