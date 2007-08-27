#ifndef TWL_COLOR_H
#define TWL_COLOR_H


#include <stdint.h>


namespace twl
{


typedef enum _EColorFormat
{
    cfUNKNOWN       = 0
    // (A/X)RGB
  , cfR8G8B8        = 1
  , cfA8R8G8B8      = 2
  , cfX8R8G8B8      = 3
  , cfR5G6B5        = 4
  , cfX1R5G5B5      = 5
  , cfA1R5G5B5      = 6
  , cfA4R4G4B4      = 7
  , cfR3G3B2        = 8
  , cfA8R3G3B2      = 9
  , cfX4R4G4B4      = 10
    // (A/X)BGR
  , cfA2B10G10R10   = 11
  , cfA8B8G8R8      = 12
  , cfX8B8G8R8      = 13
  , cfG16R16        = 14
  , cfA2R10G10B10   = 15
  , cfA16B16G16R16  = 16
    // Floating Point
  , cfA16B16G16R16F = 17
  , cfA32B32G32R32F = 18
} EColorFormat;


typedef uint16_t pixel_t;
#define ALPHA_RSHIFT 0
#define ALPHA_LSHIFT 8
#define ALPHA_MASK  0x8000
#define RED_RSHIFT 3
#define RED_LSHIFT 0
#define RED_MASK  0x001f
#define GREEN_RSHIFT 0
#define GREEN_LSHIFT 2
#define GREEN_MASK  0x03e0
#define BLUE_RSHIFT 0
#define BLUE_LSHIFT 7
#define BLUE_MASK  0x7c00
/*
typedef uint32_t pixel_t;
#define ALPHA_RSHIFT 0
#define ALPHA_LSHIFT 24
#define ALPHA_MASK  0xff000000
#define RED_RSHIFT 0
#define RED_LSHIFT 0
#define RED_MASK  0x000000ff
#define GREEN_RSHIFT 0
#define GREEN_LSHIFT 8
#define GREEN_MASK  0x0000ff00
#define BLUE_RSHIFT 0
#define BLUE_LSHIFT 16
#define BLUE_MASK  0x00ff0000
*/

typedef pixel_t  color_t;


#define BxRGB(r,g,b)     BxARGB(255,r,g,b)
#define BxARGB(a,r,g,b)  (((a<<ALPHA_LSHIFT>>ALPHA_RSHIFT)&ALPHA_MASK)|((r<<RED_LSHIFT>>RED_RSHIFT)&RED_MASK)|((g<<GREEN_LSHIFT>>GREEN_RSHIFT)&GREEN_MASK)|((b<<BLUE_LSHIFT>>BLUE_RSHIFT)&BLUE_MASK))
#define BxARGB_GET_A(c)  (255)//((c&ALPHA_MASK)>>ALPHA_LSHIFT)
#define BxARGB_GET_R(c)  ((c&RED_MASK)>>RED_LSHIFT)
#define BxARGB_GET_G(c)  ((c&GREEN_MASK)>>GREEN_LSHIFT)
#define BxARGB_GET_B(c)  ((c&BLUE_MASK)>>BLUE_LSHIFT)

#define clBlack BxRGB(0,0,0)
#define clDGray BxRGB(64,64,64)
#define clGray  BxRGB(128,128,128)
#define clLGray BxRGB(192,192,192)
#define clWhite BxRGB(255,255,255)

#define clRed   BxRGB(255,0,0)
#define clGreen BxRGB(0,255,0)
#define clBlue  BxRGB(0,0,255)


// Default desktop color
extern color_t clDesktop;
// Window title-bar colors
extern color_t clActiveWindowL;
extern color_t clActiveWindowR;
extern color_t clActiveWindow;
extern color_t clInActiveWindowL;
extern color_t clInActiveWindowR;
extern color_t clInActiveWindow;
// Panel colors
extern color_t clPanelLight;
extern color_t clPanelShade1;
extern color_t clPanelShade2;
extern color_t clPanelFill;


};


#endif
