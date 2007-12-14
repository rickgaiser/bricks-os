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
    // Palettized
  , cfP8            = 17  // 256 colors (8 bit)
  , cfP4            = 18  //  16 colors (4 bit)
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
// Convert color formats
#define BxColorFormat_Convert(fmtFrom,fmtTo,color) \
  BxColorFormat_FromRGBA(fmtTo, \
                         BxColorFormat_GetR(fmtFrom, color), \
                         BxColorFormat_GetG(fmtFrom, color), \
                         BxColorFormat_GetB(fmtFrom, color), \
                         BxColorFormat_GetA(fmtFrom, color))

//---------------------------------------------------------------------------
enum ESurfaceType
{
    stSCREEN
  , stOFFSCREEN
};

//---------------------------------------------------------------------------
typedef struct SVideoMode
{
  uint32_t xpitch;           // Framebuffer width
  uint32_t ypitch;           // Framebuffer height
  uint32_t width;            // Visible width
  uint32_t height;           // Visible height
  uint32_t bpp;              // Bits per pixel
  EColorFormat format;       // Pixel format
} SVideoMode;

//---------------------------------------------------------------------------
class CSurface
{
public:
  CSurface();
  virtual ~CSurface();

  // Video mode for surface
  virtual uint32_t xpitch();
  virtual uint32_t ypitch();
  virtual uint32_t width();
  virtual uint32_t height();
  virtual uint32_t bpp();
  virtual EColorFormat format();

//protected:
public:
  // Data
  void * p;

  // Video mode for surface
  SVideoMode mode;
};

//---------------------------------------------------------------------------
class IRenderer
{
public:
  virtual ~IRenderer(){}

  // Surfaces
  virtual void       setSurface(CSurface * surf) = 0;
  virtual CSurface * getSurface() = 0;

  // Flush operations to surface
  virtual void       flush() = 0;
};

//---------------------------------------------------------------------------
class I2DRenderer
 : public IRenderer
{
public:
  virtual ~I2DRenderer(){}

  // Surfaces
  virtual void       setSurface(CSurface * surf) = 0;
  virtual CSurface * getSurface() = 0;

  // Flush operations to surface
  virtual void       flush() = 0;

  // Color
  virtual void       setColor(uint8_t r, uint8_t g, uint8_t b) = 0;

  // Drawing
  virtual void       setPixel(int x, int y) = 0;
  virtual void       fill() = 0;
  virtual void       fillRect(int x, int y, unsigned int width, unsigned int height) = 0;
  virtual void       drawLine(int x1, int y1, int x2, int y2) = 0;
  virtual void       drawRect(int x, int y, unsigned int width, unsigned int height) = 0;
};

//---------------------------------------------------------------------------
class C2DRenderer
 : public I2DRenderer
{
public:
  C2DRenderer(CSurface * surf = 0);
  virtual ~C2DRenderer();

  // Surfaces
  virtual void       setSurface(CSurface * surf);
  virtual CSurface * getSurface();

  // Flush operations to surface
  virtual void       flush();

  // Color
  virtual void       setColor(uint8_t r, uint8_t g, uint8_t b);

  // Drawing
  virtual void       setPixel(int x, int y);
  virtual void       fill();
  virtual void       fillRect(int x, int y, unsigned int width, unsigned int height);
  virtual void       drawLine(int x1, int y1, int x2, int y2);
  virtual void       drawRect(int x, int y, unsigned int width, unsigned int height);

protected:
  // Drawing
  virtual void       setPixel_i(int x, int y);
  virtual void       fill_i();
  virtual void       fillRect_i(int x, int y, unsigned int width, unsigned int height);
  virtual void       drawLine_i(int x1, int y1, int x2, int y2);
  virtual void       drawRect_i(int x, int y, unsigned int width, unsigned int height);

protected:
  // Surface we're currently rendering on
  CSurface * pSurface_;

  // Current drawing color
  SColor color_;      // Color
  color_t fmtColor_;  // Pre calculated surface format color
};

//---------------------------------------------------------------------------
class CAVideoDevice
{
public:
  CAVideoDevice();
  virtual ~CAVideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount) = 0;
  virtual void getMode(SVideoMode ** mode) = 0;
  virtual void setMode(const SVideoMode * mode) = 0;

  virtual void getSurface(CSurface ** surface, ESurfaceType type) = 0;
  virtual void getRenderer(I2DRenderer ** renderer) = 0;

  virtual void waitVSync() = 0;
  virtual void displaySurface(CSurface * surface) = 0;

  virtual void setVSync(bool vsync = true);
  virtual bool getVSync();

protected:
  bool vSync_;
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
