#include "kernel/videoManager.h"
#include "string.h"
#include "stdlib.h"


//---------------------------------------------------------------------------
#define SET_PIXEL(x,y,c) \
{ \
  switch(colorFormatOps[format_].bitsPerPixel) \
  { \
    case 16: \
      ((uint16_t *)p)[y * width_ + x] = c; \
      break; \
    case 32: \
      ((uint32_t *)p)[y * width_ + x] = c; \
      break; \
    default: \
      ; \
  }; \
}

//---------------------------------------------------------------------------
CVideoManager videoManager;

const SColorFormatOperations colorFormatOps[] =
{
    {32, 8, 8, 8, 8, 0, 0, 0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000}  // cfUNKNOWN
    // (A/X)RGB (32bit)
  , {32, 0, 0, 0, 8,16, 8, 0, 0, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000}  // cfR8G8B8
  , {32, 0, 0, 0, 0,16, 8, 0,24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}  // cfA8R8G8B8
  , {32, 0, 0, 0, 0,16, 8, 0,24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}  // cfX8R8G8B8
    // (A/X)BGR (32bit)
  , {32, 0, 0, 0, 8, 0, 8,16,0,  0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000}  // cfB8G8R8
  , {32, 0, 0, 0, 0, 0, 8,16,24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000}  // cfA8B8G8R8
  , {32, 0, 0, 0, 0, 0, 8,16,24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000}  // cfX8B8G8R8
    // (A/X)RGB (16bit)
  , {16, 3, 2, 3, 8,11, 5, 0, 0,     0xf800,     0x07e0,     0x001f,     0x0000}  // cfR5G6B5
  , {16, 3, 3, 3, 7,10, 5, 0,15,     0x7c00,     0x03e0,     0x001f,     0x8000}  // cfA1R5G5B5
  , {16, 3, 3, 3, 7,10, 5, 0,15,     0x7c00,     0x03e0,     0x001f,     0x8000}  // cfX1R5G5B5
  , {16, 4, 4, 4, 4, 8, 4, 0,12,     0x0f00,     0x00f0,     0x000f,     0xf000}  // cfA4R4G4B4
  , {16, 4, 4, 4, 4, 8, 4, 0,12,     0x0f00,     0x00f0,     0x000f,     0xf000}  // cfX4R4G4B4
    // (A/X)BGR (16bit)
  , {16, 3, 2, 3, 8, 0, 5,11, 0,     0x001f,     0x07e0,     0xf800,     0x0000}  // cfB5G6R5
  , {16, 3, 3, 3, 7, 0, 5,10,15,     0x001f,     0x03e0,     0x7c00,     0x8000}  // cfA1B5G5R5 // NDS
  , {16, 3, 3, 3, 7, 0, 5,10,15,     0x001f,     0x03e0,     0x7c00,     0x8000}  // cfX1B5G5R5 // GBA
  , {16, 4, 4, 4, 4, 0, 4, 8,12,     0x000f,     0x00f0,     0x0f00,     0xf000}  // cfA4B4G4R4
  , {16, 4, 4, 4, 4, 0, 4, 8,12,     0x000f,     0x00f0,     0x0f00,     0xf000}  // cfX4B4G4R4
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CSurface::CSurface()
 : p(0)
 , pFront(0)
 , pBack(0)
{
  setColor(0, 0, 0);
  setFillColor(0, 0, 0);
}

//---------------------------------------------------------------------------
CSurface::~CSurface()
{
}

//---------------------------------------------------------------------------
uint32_t
CSurface::width()
{
  return width_;
}

//---------------------------------------------------------------------------
uint32_t
CSurface::height()
{
  return height_;
}

//---------------------------------------------------------------------------
void
CSurface::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  color_.r = r;
  color_.g = g;
  color_.b = b;
  color_.a = 255;

  fmtColor_ = BxColorFormat_FromRGB(format_, r, g, b);
}

//---------------------------------------------------------------------------
void
CSurface::setFillColor(uint8_t r, uint8_t g, uint8_t b)
{
  fillColor_.r = r;
  fillColor_.g = g;
  fillColor_.b = b;
  fillColor_.a = 255;

  fmtFillColor_ = BxColorFormat_FromRGB(format_, r, g, b);
}

//---------------------------------------------------------------------------
void
CSurface::setPixel(int x, int y)
{
  SET_PIXEL(x, y, fmtColor_);
}

//---------------------------------------------------------------------------
void
CSurface::fill()
{
  fillRect(0, 0, width_, height_);
}

//---------------------------------------------------------------------------
void
CSurface::fillRect(int x, int y, int width, int height)
{
  for(int iY(y); iY < (y + height); iY++)
  {
    for(int iX(x); iX < (x + width); iX++)
      SET_PIXEL(iX, iY, fmtFillColor_);
  }
}

//---------------------------------------------------------------------------
void
CSurface::drawLine(int x1, int y1, int x2, int y2)
{
  if(abs(x2 - x1) >= abs(y2 - y1))
  {
    float currenty = y1;
    float slopey = (float)(y2 - y1) / (float)(x2 - x1);
    if(x1 < x2)
    {
      for(int x(x1); x <= x2; x++)
      {
        SET_PIXEL(x, (int)currenty, fmtColor_);
        currenty += slopey;
      }
    }
    else
    {
      for(int x(x2); x <= x1; x++)
      {
        SET_PIXEL(x, (int)currenty, fmtColor_);
        currenty += slopey;
      }
    }
  }
  else
  {
    float currentx = x1;
    float slopex = (float)(x2 - x1) / (float)(y2 - y1);
    if(y1 < y2)
    {
      for(int y(y1); y <= y2; y++)
      {
        SET_PIXEL((int)currentx, y, fmtColor_);
        currentx += slopex;
      }
    }
    else
    {
      for(int y(y2); y <= y1; y++)
      {
        SET_PIXEL((int)currentx, y, fmtColor_);
        currentx += slopex;
      }
    }
  }
}

//---------------------------------------------------------------------------
void
CSurface::drawRect(int x, int y, int width, int height)
{
  drawLine(x,         y,          x + width, y);
  drawLine(x + width, y + 1,      x + width, y + height - 1);
  drawLine(x,         y + height, x + width, y + height);
  drawLine(x,         y + 1,      x,         y + height - 1);
}

/*
//---------------------------------------------------------------------------
void
CSurface::copy(int dstx, int dsty, const CSurface & src)
{
}

//---------------------------------------------------------------------------
void
CSurface::copyRect(int srcx, int srcy, int dstx, int dsty, int width, int height)
{
}
*/

//---------------------------------------------------------------------------
void
CSurface::swap(bool sync)
{
  if(pBack != 0)
  {
    if(sync == true)
      waitVSync();

    memcpy(pFront, pBack, width_ * height_ * (colorFormatOps[format_].bitsPerPixel / 8));
  }
}

//---------------------------------------------------------------------------
void
CSurface::waitHSync()
{
  // No HSync
}

//---------------------------------------------------------------------------
void
CSurface::waitVSync()
{
  // No VSync
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAVideoDevice::CAVideoDevice()
{
  videoManager.addDevice(this);
}

//---------------------------------------------------------------------------
CAVideoDevice::~CAVideoDevice()
{
  videoManager.removeDevice(this);
}

//---------------------------------------------------------------------------
void
CAVideoDevice::listModes(const SVideoMode ** modes, int * modeCount)
{
  *modes = 0;
  *modeCount = 0;
}

//---------------------------------------------------------------------------
void
CAVideoDevice::getMode(SVideoMode ** mode)
{
  *mode = 0;
}

//---------------------------------------------------------------------------
void
CAVideoDevice::setMode(const SVideoMode * mode)
{
}

//---------------------------------------------------------------------------
void
CAVideoDevice::getSurface(CSurface ** surface, ESurfaceType type)
{
  *surface = 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CVideoManager::CVideoManager()
 : iDeviceCount_(0)
{
  for(int i(0); i < MAX_VIDEO_DEVICE_COUNT; i++)
    devices_[i] = 0;
}

//---------------------------------------------------------------------------
CVideoManager::~CVideoManager()
{
}

//---------------------------------------------------------------------------
void
CVideoManager::listDevices(CAVideoDevice ** devices[], int * deviceCount)
{
  *devices = devices_;
  *deviceCount = iDeviceCount_;
}

//---------------------------------------------------------------------------
void
CVideoManager::addDevice(CAVideoDevice * device)
{
  for(int i(0); i < MAX_VIDEO_DEVICE_COUNT; i++)
  {
    if(devices_[i] == 0)
    {
      devices_[i] = device;
      iDeviceCount_++;
      break;
    }
  }
}

//---------------------------------------------------------------------------
void
CVideoManager::removeDevice(CAVideoDevice * device)
{
  for(int i(0); i < MAX_VIDEO_DEVICE_COUNT; i++)
  {
    if(devices_[i] == device)
    {
      devices_[i] = 0;
      iDeviceCount_--;
      break;
    }
  }
}
