#ifndef KERNEL_VIDEOMANAGER_H
#define KERNEL_VIDEOMANAGER_H


#include "inttypes.h"


//---------------------------------------------------------------------------
// Main color format type, can hold any color <= 32 bits
typedef uint32_t color_t;
// Color struct
struct SColor
{
  uint8_t r, g, b, a;
};

//---------------------------------------------------------------------------
struct SColorFormatOperations
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
};
extern const SColorFormatOperations colorFormatOps[];

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
// Create color formats
#define BxColorFormat_FromRGBA(fmt,r,g,b,a) \
  ((r >> colorFormatOps[fmt].lossR) << colorFormatOps[fmt].shiftR) | \
  ((g >> colorFormatOps[fmt].lossG) << colorFormatOps[fmt].shiftG) | \
  ((b >> colorFormatOps[fmt].lossB) << colorFormatOps[fmt].shiftB) | \
  ((a >> colorFormatOps[fmt].lossA) << colorFormatOps[fmt].shiftA)
#define BxColorFormat_FromRGB(fmt,r,g,b) \
  ((r >> colorFormatOps[fmt].lossR) << colorFormatOps[fmt].shiftR) | \
  ((g >> colorFormatOps[fmt].lossG) << colorFormatOps[fmt].shiftG) | \
  ((b >> colorFormatOps[fmt].lossB) << colorFormatOps[fmt].shiftB) | \
  (colorFormatOps[fmt].maskA)
// Get colors from color formats
#define BxColorFormat_GetR(fmt,color) \
  (((color & colorFormatOps[fmt].maskR) >> colorFormatOps[fmt].shiftR) << colorFormatOps[fmt].lossR)
#define BxColorFormat_GetG(fmt,color) \
  (((color & colorFormatOps[fmt].maskG) >> colorFormatOps[fmt].shiftG) << colorFormatOps[fmt].lossG)
#define BxColorFormat_GetB(fmt,color) \
  (((color & colorFormatOps[fmt].maskB) >> colorFormatOps[fmt].shiftB) << colorFormatOps[fmt].lossB)
#define BxColorFormat_GetA(fmt,color) \
  (((color & colorFormatOps[fmt].maskA) >> colorFormatOps[fmt].shiftA) << colorFormatOps[fmt].lossA)

//---------------------------------------------------------------------------
enum ESurfaceType
{
    stSCREEN
  , stOFFSCREEN
};

//---------------------------------------------------------------------------
struct SVideoMode
{
  int width;
  int height;
  int bpp;
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
  virtual void     fillRect(int x, int y, unsigned int width, unsigned int height);
  virtual void     drawLine(int x1, int y1, int x2, int y2);
  virtual void     drawRect(int x, int y, unsigned int width, unsigned int height);
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

protected:
  virtual void     setPixel_i(int x, int y);
  virtual void     fillRect_i(int x, int y, unsigned int width, unsigned int height);
  virtual void     drawLine_i(int x1, int y1, int x2, int y2);
  virtual void     drawRect_i(int x, int y, unsigned int width, unsigned int height);

//protected:
public:
  // Data
  void * p;
  void * pFront;
  void * pBack;

  // Geometry
  unsigned int width_;
  unsigned int height_;

  // Colors
  SColor color_;
  SColor fillColor_;

  // Surface format colors
  EColorFormat format_;
  color_t fmtColor_;
  color_t fmtFillColor_;
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

  virtual void getSurface(CSurface ** surface, ESurfaceType type, bool bDouble);
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
