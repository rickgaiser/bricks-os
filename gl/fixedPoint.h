#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H


#include "GLES/gl.h"
#include "inttypes.h"


// Conversions
#define fpfromi(c,a)       ((a)<<(c))
#define fpfromf(c,a)       ((GLfixed)((a)*(1<<(c))))
#define fptoi(c,a)         ((a)>>(c))
#define fptof(c,a)         ((float)(a)/(1<<(c)))
#define fp32to64(c,a)      (((int64_t)a)<<(c))
#define fp64to32(c,a)      ((a)>>(c))
#define fp16to32(c,a)      (((int32_t)a)<<(c))
#define fp32to16(c,a)      ((a)>>(c))

// Math (Use at own risk)
#define fpmul(c,a,b)       (((a)*(b))>>(c))
#define fpdiv(c,a,b)       (((a)<<(c))/(b))
// Math (16 bit)
#define fpmul16(c,a,b)     ((((int32_t)a)*((int32_t)b))>>(c))
#define fpdiv16(c,a,b)     ((((int32_t)a)<<(c))/(b))
#define fpmul16to32(c,a,b) (((int32_t)a)*((int32_t)b))
// Math (32 bit)
#define fpmul32(c,a,b)     ((((int64_t)a)*((int64_t)b))>>(c))
#define fpdiv32(c,a,b)     ((((int64_t)a)<<(c))/(b))
#define fpmul32to64(c,a,b) (((int64_t)a)*((int64_t)b))


#endif
