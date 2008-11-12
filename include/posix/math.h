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


#ifndef MATH_H
#define MATH_H


#include "_math.h"


#ifdef __cplusplus
extern "C"
{
#endif


//double acos  (double x);
//double asin  (double x);
//double atan  (double x);
//double atan2 (double y, double x);
double ceil  (double x);
double cos   (double x);
//double cosh  (double x);
//double exp   (double x);
double fabs  (double x);
double floor (double x);
//double fmod  (double x, double y);
//double frexp (double fmod, int * exp);
//double ldexp (double x, int exp);
//double log   (double x);
//double log10 (double x);
//double modf  (double x, double * iptr);
//double pow   (double x, double y);
double sin   (double x);
//double sinh  (double x);
double sqrt  (double x);
double tan   (double x);
//double tanh  (double x);
//double erf   (double x);
//double erfc  (double x);
//double gamma (double x);
//double hypot (double x, double y);
//double j0    (double x);
//double j1    (double x);
//double jn    (int n, double x);
//double lgamma(double x);
//double y0    (double x);
//double y1    (double x);
//double yn    (int n, double x);
//int    isnan (double x);
//double acosh (double x);
//double asinh (double x);
//double atanh (double x);
//double cbrt  (double x);
//double expm1 (double x);
//int    ilogb (double x);
//double log1p (double x);
//double logb  (double x);
//double nextafter(double x, double y);
//double remainder(double x, double y);
//double rint  (double x);
//double scalb (double x, double n);

// Extra functions
float  cosf  (float  x);
float  sinf  (float  x);
float  tanf  (float  x);
float  sqrtf (float  x);
double inv_sqrt(double x);
float  inv_sqrtf(float  x);


#ifdef __cplusplus
}
#endif


#endif
