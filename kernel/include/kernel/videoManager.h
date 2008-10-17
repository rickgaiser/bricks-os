#ifndef KERNEL_VIDEOMANAGER_H
#define KERNEL_VIDEOMANAGER_H


#include "inttypes.h"
#include "stddef.h"


//---------------------------------------------------------------------------
// Main color format type (cfA8R8G8B8)
typedef uint32_t color_t;

// Color struct
typedef union
{
  struct
  {
    uint8_t a, r, g, b;
  } __attribute__ ((__packed__));
  color_t color;
} SColor;

// Color class (same as struct, but with constructors)
class CColor
{
public:
  inline CColor(color_t _color = 0) : color(_color) {}
  inline CColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255) : a(_a), r(_r), g(_g), b(_b) {}

public:
  union
  {
    struct
    {
      uint8_t a, r, g, b;
    } __attribute__ ((__packed__));
    color_t color;
  };
};

//---------------------------------------------------------------------------
struct SColorFormatOperations
{
  uint8_t  bitsPerPixel;
  // Shift right 8bit value when creating color
  uint8_t  lossR;
  uint8_t  lossG;
  uint8_t  lossB;
  uint8_t  lossA;
  // Shift left value into color format position
  uint8_t  shiftR;
  uint8_t  shiftG;
  uint8_t  shiftB;
  uint8_t  shiftA;
  // Mask for picking a color from a color format
  uint32_t maskR;
  uint32_t maskG;
  uint32_t maskB;
  uint32_t maskA;
};
extern const SColorFormatOperations colorFormatOps[];

//---------------------------------------------------------------------------
struct SColorFormatConverter
{
  // Source color mask
  uint32_t maskR;
  uint32_t maskG;
  uint32_t maskB;
  uint32_t maskA;
  // Right shift color
  uint8_t  rshiftR;
  uint8_t  rshiftG;
  uint8_t  rshiftB;
  uint8_t  rshiftA;
  // Left shift color
  uint8_t  lshiftR;
  uint8_t  lshiftG;
  uint8_t  lshiftB;
  uint8_t  lshiftA;
};

//---------------------------------------------------------------------------
enum EColorFormat
{
    cfUNKNOWN       =  0
    // (A/X)RGB(A/X) (32bit)
  , cfA8R8G8B8      =  1
  , cfX8R8G8B8      =  2
  , cfR8G8B8A8      =  3
  , cfR8G8B8X8      =  4
    // (A/X)BGR(A/X) (32bit)
  , cfA8B8G8R8      =  5
  , cfX8B8G8R8      =  6
  , cfB8G8R8A8      =  7
  , cfB8G8R8X8      =  8
  // RGB/BGR (24bit)
  , cfR8G8B8        =  9
  , cfB8G8R8        = 10
    // (A/X)RGB(A/X) (16bit)
  , cfR5G6B5        = 11
  , cfA1R5G5B5      = 12
  , cfX1R5G5B5      = 13
  , cfA4R4G4B4      = 14
  , cfX4R4G4B4      = 15
  , cfR5G5B5A1      = 16
  , cfR5G5B5X1      = 17
  , cfR4G4B4A4      = 18
  , cfR4G4B4X4      = 19
    // (A/X)BGR(A/X) (16bit)
  , cfB5G6R5        = 20
  , cfA1B5G5R5      = 21
  , cfX1B5G5R5      = 22
  , cfA4B4G4R4      = 23
  , cfX4B4G4R4      = 24
  , cfB5G5R5A1      = 25
  , cfB5G5R5X1      = 26
  , cfB4G4R4A4      = 27
  , cfB4G4R4X4      = 28
    // Palettized RGB (8bit)
  , cfR3G3B2        = 29
    // Palettized
  , cfP8            = 30 // 256 colors (8 bit)
  , cfP4            = 31 //  16 colors (4 bit)
};

//---------------------------------------------------------------------------
// Create color formats
#define BxColorFormat_FromRGBA(fmt,r,g,b,a) \
  (((r >> colorFormatOps[fmt].lossR) << colorFormatOps[fmt].shiftR) | \
   ((g >> colorFormatOps[fmt].lossG) << colorFormatOps[fmt].shiftG) | \
   ((b >> colorFormatOps[fmt].lossB) << colorFormatOps[fmt].shiftB) | \
   ((a >> colorFormatOps[fmt].lossA) << colorFormatOps[fmt].shiftA))
#define BxColorFormat_FromRGB(fmt,r,g,b) \
  (((r >> colorFormatOps[fmt].lossR) << colorFormatOps[fmt].shiftR) | \
   ((g >> colorFormatOps[fmt].lossG) << colorFormatOps[fmt].shiftG) | \
   ((b >> colorFormatOps[fmt].lossB) << colorFormatOps[fmt].shiftB))
// Get colors from color formats
#define BxColorFormat_GetR(fmt,color) \
  (((color & colorFormatOps[fmt].maskR) >> colorFormatOps[fmt].shiftR) << colorFormatOps[fmt].lossR)
#define BxColorFormat_GetG(fmt,color) \
  (((color & colorFormatOps[fmt].maskG) >> colorFormatOps[fmt].shiftG) << colorFormatOps[fmt].lossG)
#define BxColorFormat_GetB(fmt,color) \
  (((color & colorFormatOps[fmt].maskB) >> colorFormatOps[fmt].shiftB) << colorFormatOps[fmt].lossB)
#define BxColorFormat_GetA(fmt,color) \
  (((color & colorFormatOps[fmt].maskA) >> colorFormatOps[fmt].shiftA) << colorFormatOps[fmt].lossA)
// Create color format conversion struct
void BxColorFormat_CreateConverter(SColorFormatConverter & conv, EColorFormat from, EColorFormat to);
// Convert color formats
#define BxColorFormat_ConvertRGB(conv,color) \
  ((((color & conv.maskR) >> conv.rshiftR) << conv.lshiftR) | \
   (((color & conv.maskG) >> conv.rshiftG) << conv.lshiftG) | \
   (((color & conv.maskB) >> conv.rshiftB) << conv.lshiftB))
#define BxColorFormat_ConvertRGBA(conv,color) \
  ((((color & conv.maskR) >> conv.rshiftR) << conv.lshiftR) | \
   (((color & conv.maskG) >> conv.rshiftG) << conv.lshiftG) | \
   (((color & conv.maskB) >> conv.rshiftB) << conv.lshiftB) | \
   (((color & conv.maskA) >> conv.rshiftA) << conv.lshiftA))

//---------------------------------------------------------------------------
enum ESurfaceType
{
    stSCREEN
  , stOFFSCREEN
};

//---------------------------------------------------------------------------
// Video Modes
#define VM_PROGRESSIVE      0x00000001 // Full Frames, best quality
#define VM_INTERLACED       0x00000002 // Very efficient when double buffering, but keep up the framerate!
#define VM_FULL_INTERLACED  0x00000004 // Uses Full Frames (good for double buffering when framerate can't keep up)

//---------------------------------------------------------------------------
typedef struct SVideoMode
{
  uint32_t xpitch;           // Framebuffer width
  uint32_t ypitch;           // Framebuffer height
  uint32_t width;            // Visible width
  uint32_t height;           // Visible height
  uint32_t bpp;              // Bits per pixel
//  uint16_t aRatioW;          // Aspect Ratio Width
//  uint16_t aRatioH;          // Aspect Ratio Height
  EColorFormat format;       // Pixel format
//  uint32_t flags;
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
  IRenderer() : renderSurface(NULL) {}
  virtual ~IRenderer() {}

  // Surfaces
  virtual void       setSurface(CSurface * surface){renderSurface = surface;}
  virtual CSurface * getSurface()                  {return renderSurface;}

  // Flush operations to surface
  virtual void       flush()                       {}

protected:
  CSurface  * renderSurface;
};

//---------------------------------------------------------------------------
class I2DRenderer;
class I3DRenderer;
//---------------------------------------------------------------------------
class CAVideoDevice
{
public:
  CAVideoDevice();
  virtual ~CAVideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount) = 0;
  virtual void getCurrentMode(const SVideoMode ** mode) = 0;
  virtual void getDefaultMode(const SVideoMode ** mode) = 0;
  virtual void setMode(const SVideoMode * mode) = 0;

  virtual void getSurface(CSurface ** surface, int width, int height) = 0;
  virtual void get2DRenderer(I2DRenderer ** renderer) = 0;
  virtual void get3DRenderer(I3DRenderer ** renderer) = 0;

  // Screen positioning (in pixels)
  virtual bool     hasPositioning(){return false;}
  virtual uint16_t getMaxHorizontalOffset(){return 0;}
  virtual uint16_t getHorizontalOffset(){return 0;}
  virtual void     setHorizontalOffset(uint16_t x){}
  virtual uint16_t getMaxVerticalOffset(){return 0;}
  virtual uint16_t getVerticalOffset(){return 0;}
  virtual void     setVerticalOffset(uint16_t y){}

  virtual uint32_t getFrameNr() = 0; // Returns current frame/field number
  virtual uint32_t waitVSync() = 0; // Returns current frame/field number, after vsync
  virtual void displaySurface(CSurface * surface) = 0;

  virtual void bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy);
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
  void bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy);

private:
  void addDevice(CAVideoDevice * device);
  void removeDevice(CAVideoDevice * device);

  CAVideoDevice * devices_[MAX_VIDEO_DEVICE_COUNT];
  int iDeviceCount_;
};


extern CVideoManager videoManager;


#endif
