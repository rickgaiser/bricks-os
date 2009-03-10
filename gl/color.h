#ifndef RASTER_COLOR_H
#define RASTER_COLOR_H


#include "bitResolution.h"


struct SRasterColor
{
  int32_t r;
  int32_t g;
  int32_t b;
  int32_t a;
};


// Averaging
#define COLOR_AVG_COMP(c0,c1,t) ((c0) + ((((c1) - (c0)) * (t)) >> SHIFT_COLOR))
#define COLOR_AVG(cto,c0,c1,t) \
  cto.r = COLOR_AVG_COMP(c0.r,c1.r,t); \
  cto.g = COLOR_AVG_COMP(c0.g,c1.g,t); \
  cto.b = COLOR_AVG_COMP(c0.b,c1.b,t); \
  cto.a = COLOR_AVG_COMP(c0.a,c1.a,t)
// Multiply
#define COLOR_MUL_COMP(c0,c1) ((c0 * c1) >> SHIFT_COLOR)
#define COLOR_MUL(cto,c0,c1) \
  cto.r = COLOR_MUL_COMP(c0.r,c1.r); \
  cto.g = COLOR_MUL_COMP(c0.g,c1.g); \
  cto.b = COLOR_MUL_COMP(c0.b,c1.b); \
  cto.a = COLOR_MUL_COMP(c0.a,c1.a)
// Add
#define COLOR_ADD_COMP(c0,c1) (c0 + c1)
#define COLOR_ADD(cto,c0,c1) \
  cto.r = COLOR_ADD_COMP(c0.r,c1.r); \
  cto.g = COLOR_ADD_COMP(c0.g,c1.g); \
  cto.b = COLOR_ADD_COMP(c0.b,c1.b); \
  cto.a = COLOR_ADD_COMP(c0.a,c1.a)
// Clamp
#define COLOR_CLAMP_TOP_COMP(c) ((c > (1<<SHIFT_COLOR)) ? (1<<SHIFT_COLOR) : c)
#define COLOR_CLAMP_TOP(cto,c) \
  cto.r = COLOR_CLAMP_TOP_COMP(c.r); \
  cto.g = COLOR_CLAMP_TOP_COMP(c.g); \
  cto.b = COLOR_CLAMP_TOP_COMP(c.b); \
  cto.a = COLOR_CLAMP_TOP_COMP(c.a)
#define COLOR_CLAMP_COMP(c) ((c < 0) ? 0 : ((c > (1<<SHIFT_COLOR)) ? (1<<SHIFT_COLOR) : c))
#define COLOR_CLAMP(cto,c) \
  cto.r = COLOR_CLAMP_COMP(c.r); \
  cto.g = COLOR_CLAMP_COMP(c.g); \
  cto.b = COLOR_CLAMP_COMP(c.b); \
  cto.a = COLOR_CLAMP_COMP(c.a)

// Load colors
#define COLOR_LOAD_555(c,t) \
  c.b = ((t & 0x7c00) >> 10) * ((1<<SHIFT_COLOR) / 31); \
  c.g = ((t & 0x03e0) >>  5) * ((1<<SHIFT_COLOR) / 31); \
  c.r = ((t & 0x001f)      ) * ((1<<SHIFT_COLOR) / 31); \
  c.a = (1<<SHIFT_COLOR)
#define COLOR_LOAD_565(c,t) \
  c.r = ((t & 0xf800) >> 11) * ((1<<SHIFT_COLOR) / 31); \
  c.g = ((t & 0x07e0) >>  5) * ((1<<SHIFT_COLOR) / 63); \
  c.b = ((t & 0x001f)      ) * ((1<<SHIFT_COLOR) / 31); \
  c.a = (1<<SHIFT_COLOR)
#define COLOR_LOAD_888(c,t) \
  c.r = ((t & 0x00ff0000) >> 16) * ((1<<SHIFT_COLOR) / 255); \
  c.g = ((t & 0x0000ff00) >>  8) * ((1<<SHIFT_COLOR) / 255); \
  c.b = ((t & 0x000000ff)      ) * ((1<<SHIFT_COLOR) / 255); \
  c.a = (1<<SHIFT_COLOR)
#define COLOR_LOAD_8888(c,t) \
  c.r = ((t & 0x00ff0000) >> 16) * ((1<<SHIFT_COLOR) / 255); \
  c.g = ((t & 0x0000ff00) >>  8) * ((1<<SHIFT_COLOR) / 255); \
  c.b = ((t & 0x000000ff)      ) * ((1<<SHIFT_COLOR) / 255); \
  c.a = ((t             ) >> 24) * ((1<<SHIFT_COLOR) / 255)

// Save colors
#define COLOR_SAVE_COMP_5(c) (((((int32_t)(c))* 31) & (0x1f<<SHIFT_COLOR))>>SHIFT_COLOR)
#define COLOR_SAVE_COMP_6(c) (((((int32_t)(c))* 63) & (0x3f<<SHIFT_COLOR))>>SHIFT_COLOR)
#define COLOR_SAVE_COMP_8(c) (((((int32_t)(c))*255) & (0xff<<SHIFT_COLOR))>>SHIFT_COLOR)
#define COLOR_SAVE_555(c) \
  ((COLOR_SAVE_COMP_5(c.b) << 10) | \
   (COLOR_SAVE_COMP_5(c.g) <<  5) | \
   (COLOR_SAVE_COMP_5(c.r)      ))
#define COLOR_SAVE_565(c) \
  ((COLOR_SAVE_COMP_5(c.r) << 11) | \
   (COLOR_SAVE_COMP_6(c.g) <<  5) | \
   (COLOR_SAVE_COMP_5(c.b)      ))
#define COLOR_SAVE_888(c) \
  ((COLOR_SAVE_COMP_8(c.r) << 16) | \
   (COLOR_SAVE_COMP_8(c.g) <<  8) | \
   (COLOR_SAVE_COMP_8(c.b)      ))
#define COLOR_SAVE_8888(c) \
  ((COLOR_SAVE_COMP_8(c.a) << 24) | \
   (COLOR_SAVE_COMP_8(c.r) << 16) | \
   (COLOR_SAVE_COMP_8(c.g) <<  8) | \
   (COLOR_SAVE_COMP_8(c.b)      ))


#endif
