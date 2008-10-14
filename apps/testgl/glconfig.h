#ifndef GLCONFIG_H
#define GLCONFIG_H


#include "asm/arch/config.h"


// Use floating point interface instead of fixed point
#ifdef CONFIG_FPU
  #define USE_FLOATING_POINT
#endif

// Enable double buffering, should be enabled on most devices
// NOTE: ngc/wii currently uses the flush function to copy from RGB to YUV
//       framebuffer. Therefore a double RGB buffer is not needed.
#define ENABLE_DOUBLE_BUFFERED

// Enable vertical sync, works on most platforms, doesn't hurt on the rest
#define ENABLE_VSYNC

// Enable fog, doesn't hurt
//#define ENABLE_FOG

// Enable ligting in test, currently experimental
#define ENABLE_LIGHTING

// Enable textures, currently experimental
#define ENABLE_TEXTURES

// Enable deph testing (z-buffer), some platforms only
//#define ENABLE_DEPTH_TEST


#endif
