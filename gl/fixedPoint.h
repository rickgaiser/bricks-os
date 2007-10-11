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

// Macro's for OpenGL (16.16)
#define FP_PRESICION_GL 16
#define gl_fpfromi(i)   fpfromi(FP_PRESICION_GL,i)
#define gl_fptoi(i)     fptoi(FP_PRESICION_GL,i)
#define gl_fpfromf(i)   fpfromf(FP_PRESICION_GL,i)
#define gl_fptof(i)     fptof(FP_PRESICION_GL,i)
#define gl_fpmul(i1,i2) fpmul32(FP_PRESICION_GL,i1,i2)
#define gl_fpdiv(i1,i2) fpdiv32(FP_PRESICION_GL,i1,i2)
#define gl_fpclamp(i)   (i < 0 ? 0 : (i > gl_fpfromi(1) ? gl_fpfromi(1) : i))


#endif
