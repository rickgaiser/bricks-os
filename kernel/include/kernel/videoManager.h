#ifndef KERNEL_VIDEOMANAGER_H
#define KERNEL_VIDEOMANAGER_H


#include "inttypes.h"


//---------------------------------------------------------------------------
// Main color type: cfA8R8G8B8
typedef uint32_t color_t;
#define BxColor(r,g,b,a) ((a<<24)|(r<<16)|(g<<8)|(b))
#define BxColor_GetA(c)  ((c&0xff000000)>>24)
#define BxColor_GetR(c)  ((c&0x00ff0000)>>16)
#define BxColor_GetG(c)  ((c&0x0000ff00)>>8)
#define BxColor_GetB(c)  ((c&0x000000ff))

//---------------------------------------------------------------------------
enum EColorFormat
{
    cfUNKNOWN       = 0
    // (A/X)RGB (32bit)
  , cfR8G8B8        = 1
  , cfA8R8G8B8      = 2
  , cfX8R8G8B8      = 3
    // (A/X)BGR (32bit)
  , cfB8G8R8        = 4
  , cfA8B8G8R8      = 5
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
};

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

  // Geometry
  virtual uint32_t width();
  virtual uint32_t height();

  // Color
  virtual void     setColor(uint8_t r, uint8_t g, uint8_t b);
  virtual void     setFillColor(uint8_t r, uint8_t g, uint8_t b);

  // Drawing
  virtual void     setPixel(int x, int y);
  virtual void     fill();
  virtual void     fillRect(int x, int y, int width, int height);
  virtual void     drawLine(int x1, int y1, int x2, int y2);
  virtual void     drawRect(int x, int y, int width, int height);
  // Copy another surface onto this surface
//  void copy(int dstx, int dsty, const CSurface & src);
  // Copy part of the surface to another part of the surface
//  void copyRect(int srcx, int srcy, int dstx, int dsty, int width, int height);

  // Swap back buffer to front buffer, only if back buffer exists
  virtual void     swap(bool sync = false);

  // Wait for Horizontal Synchronization
  virtual void     waitHSync();
  // Wait for Vertical Synchronization
  virtual void     waitVSync();

//protected:
  // Data
  void * p;
  void * pFront;
  void * pBack;

  // Geometry
  uint32_t width_;
  uint32_t height_;

  // Colors
  EColorFormat format_;
  color_t color_;
  color_t fillColor_;
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
