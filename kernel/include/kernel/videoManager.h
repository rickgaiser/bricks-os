#ifndef KERNEL_VIDEOMANAGER_H
#define KERNEL_VIDEOMANAGER_H


#include "inttypes.h"


typedef uint16_t pixel_t;
typedef pixel_t  color_t;
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


//---------------------------------------------------------------------------
typedef struct _SColorValue
{
  float r;
  float g;
  float b;
  float a;
} SColorValue;

//---------------------------------------------------------------------------
typedef enum _EColorFormat
{
    cfUNKNOWN       = 0
    // (A/X)RGB (32bit)
  , cfR8G8B8        = 1
  , cfA8R8G8B8      = 2
  , cfX8R8G8B8      = 3
    // (A/X)BGR (32bit)
  , cfB8G8R8        = 4
  , cfA8B8G8R8      = 5   // PS2
  , cfX8B8G8R8      = 6
    // (A/X)RGB (16bit)
  , cfR5G6B5        = 7
  , cfA1R5G5B5      = 8
  , cfX1R5G5B5      = 9
  , cfA4R4G4B4      = 10
  , cfX4R4G4B4      = 11
    // (A/X)BGR (16bit)
  , cfB5G6R5        = 12
  , cfA1B5G5R5      = 13  // NDS
  , cfX1B5G5R5      = 14  // GBA
  , cfA4B4G4R4      = 15
  , cfX4B4G4R4      = 16
} EColorFormat;

//---------------------------------------------------------------------------
typedef struct _SColorFormatOperations
{
  uint8_t  bitsPerPixel;
  uint8_t  lossR;
  uint8_t  lossG;
  uint8_t  lossB;
  uint8_t  lossA;
  uint8_t  shiftR;
  uint8_t  shiftG;
  uint8_t  shiftB;
  uint8_t  shiftA;
  uint32_t maskR;
  uint32_t maskG;
  uint32_t maskB;
  uint32_t maskA;
} SColorFormatOperations;

//---------------------------------------------------------------------------
#define COLOR_TO_RGB(color, fmt, r, g, b)                    \
{                                                            \
  r = (((color & fmt->maskR) >> fmt->shiftR) << fmt->lossR); \
  g = (((color & fmt->maskG) >> fmt->shiftG) << fmt->lossG); \
  b = (((color & fmt->maskB) >> fmt->shiftB) << fmt->lossB); \
}

//---------------------------------------------------------------------------
enum ESurfaceType
{
    stSCREEN
  , stOFFSCREEN
};

//---------------------------------------------------------------------------
struct SVideoMode
{
  int xres;
  int yres;
  int bitsPerPixel;
  int bytesPerPixel;
  EColorFormat format;
};

//---------------------------------------------------------------------------
class CSurface
{
public:
  CSurface();
  virtual ~CSurface();

  // Fill entire surface
  virtual void fill(color_t color);
  // Fill rect on surface
  virtual void fillRect(int x, int y, int width, int height, color_t color);
  // Copy another surface onto this surface
//  void copy(int dstx, int dsty, const CSurface & src);
  // Copy part of the surface to another part of the surface
//  void copyRect(int srcx, int srcy, int dstx, int dsty, int width, int height);
  // Swap back buffer to front buffer, only if back buffer exists
  virtual void swap(bool bForceCopy = false);

  // Geometry
  uint32_t width;
  uint32_t height;

  // Format
  EColorFormat format;
  bool key;
  color_t clKey;

  // Data
  void * p;

//protected:
  // Data
  void * pFront;
  void * pBack;
};

//---------------------------------------------------------------------------
class CAVideoDevice
{
public:
  CAVideoDevice();
  virtual ~CAVideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount);
  virtual void getMode(SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, ESurfaceType type);
};

//---------------------------------------------------------------------------
#define MAX_VIDEO_DEVICE_COUNT 2
class CVideoManager
{
public:
  friend class CAVideoDevice;

  CVideoManager();
  virtual ~CVideoManager();

  void listDevices(CAVideoDevice ** devices[], int * deviceCount);

private:
  void addDevice(CAVideoDevice * device);
  void removeDevice(CAVideoDevice * device);

  CAVideoDevice * devices_[MAX_VIDEO_DEVICE_COUNT];
  int iDeviceCount_;
};


extern CVideoManager videoManager;


#endif
