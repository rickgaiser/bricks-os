#include "kernel/videoManager.h"
#include "string.h"
#include "stdlib.h"


//---------------------------------------------------------------------------
#define SET_PIXEL(x,y,c) \
{ \
  switch(bpp_) \
  { \
    case 8: \
      ((uint8_t  *)p)[y * width_ + x] = c; \
      break; \
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
#define VISIBLE_POINT(x,y) ((x >= 0) && (x < (int)width_) && (y >= 0) && (y < (int)height_))

//---------------------------------------------------------------------------
#define VISIBLE_RECT(x,y,w,h) ((x < (int)width_) && ((x+w) >= 0) && (y < (int)height_) && ((y+h) >= 0))

//---------------------------------------------------------------------------
#define CLIP_POINT(x,y) \
{ \
  if(x < 0) x = 0; \
  if(y < 0) y = 0; \
  else if((unsigned int)x >= width_)  x = width_ - 1; \
  else if((unsigned int)y >= height_) y = height_ -1; \
}

//---------------------------------------------------------------------------
// Make sure the rect is visible first, using 'VISIBLE(x,y,w,h);'
// Otherwise this macro makes no sense
#define CLIP_RECT(x,y,w,h) \
{ \
  if(x < 0) {x = 0; width -= x;} \
  if(y < 0) {y = 0; height -= y;} \
  if((x+w) > width_) {w = width_ - x;} \
  if((y+h) > height_) {h = height_ - y;} \
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
  , {16, 3, 2, 3, 8,11, 5, 0, 0, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000}  // cfR5G6B5
  , {16, 3, 3, 3, 7,10, 5, 0,15, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000}  // cfA1R5G5B5
  , {16, 3, 3, 3, 7,10, 5, 0,15, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000}  // cfX1R5G5B5
  , {16, 4, 4, 4, 4, 8, 4, 0,12, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000}  // cfA4R4G4B4
  , {16, 4, 4, 4, 4, 8, 4, 0,12, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000}  // cfX4R4G4B4
    // (A/X)BGR (16bit)
  , {16, 3, 2, 3, 8, 0, 5,11, 0, 0x0000001f, 0x000007e0, 0x0000f800, 0x00000000}  // cfB5G6R5
  , {16, 3, 3, 3, 7, 0, 5,10,15, 0x0000001f, 0x000003e0, 0x00007c00, 0x00008000}  // cfA1B5G5R5 // NDS
  , {16, 3, 3, 3, 7, 0, 5,10,15, 0x0000001f, 0x000003e0, 0x00007c00, 0x00008000}  // cfX1B5G5R5 // GBA
  , {16, 4, 4, 4, 4, 0, 4, 8,12, 0x0000000f, 0x000000f0, 0x00000f00, 0x0000f000}  // cfA4B4G4R4
  , {16, 4, 4, 4, 4, 0, 4, 8,12, 0x0000000f, 0x000000f0, 0x00000f00, 0x0000f000}  // cfX4B4G4R4
    // Palettized
  , { 8, 8, 8, 8, 8, 0, 0, 0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000}  // cfP8
  , { 4, 8, 8, 8, 8, 0, 0, 0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000}  // cfP4
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
  if(VISIBLE_POINT(x, y))
    setPixel_i(x, y);
}

//---------------------------------------------------------------------------
void
CSurface::setPixel_i(int x, int y)
{
  SET_PIXEL(x, y, fmtColor_);
}

//---------------------------------------------------------------------------
void
CSurface::fill()
{
  fillRect_i(0, 0, width_, height_);
}

//---------------------------------------------------------------------------
void
CSurface::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  if(VISIBLE_RECT(x, y, width, height))
  {
    CLIP_RECT(x, y, width, height);
    fillRect_i(x, y, width, height);
  }
}

//---------------------------------------------------------------------------
void
CSurface::fillRect_i(int x, int y, unsigned int width, unsigned int height)
{
  for(unsigned int iY(y); iY < (y + height); iY++)
  {
    for(unsigned int iX(x); iX < (x + width); iX++)
      SET_PIXEL(iX, iY, fmtFillColor_);
  }
}

//---------------------------------------------------------------------------
void
CSurface::drawLine(int x1, int y1, int x2, int y2)
{
  // FIXME: Are we visible?
  CLIP_POINT(x1, y1);
  CLIP_POINT(x2, y2);

  // order: Smallest x first
  if(x1 <= x2)
    drawLine_i(x1, y1, x2, y2);
  else
    drawLine_i(x2, y2, x1, y1);
}

//---------------------------------------------------------------------------
void
CSurface::drawLine_i(int x1, int y1, int x2, int y2)
{
  float dx = (x2-x1);
  float dy = (y2-y1);

  if(dx >= abs(dy))
  {
    float currenty = y1 + 0.5f;
    float slopey   = dy / dx;

    for(; x1 <= x2; x1++)
    {
      SET_PIXEL(x1, (int)currenty, fmtColor_);
      currenty += slopey;
    }
  }
  else
  {
    float currentx = x1 + 0.5f;
    float slopex   = dx / dy;
    int ystart = (y1 <= y2) ? y1 : y2;
    int yend   = (y1 <= y2) ? y2 : y1;

    for(; ystart <= yend; ystart++)
    {
      SET_PIXEL((int)currentx, ystart, fmtColor_);
      currentx += slopex;
    }
  }
}

//---------------------------------------------------------------------------
void
CSurface::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  if(VISIBLE_RECT(x, y, width, height))
  {
    CLIP_RECT(x, y, width, height);
    drawRect_i(x, y, width, height);
  }
}

//---------------------------------------------------------------------------
void
CSurface::drawRect_i(int x, int y, unsigned int width, unsigned int height)
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

    memcpy(pFront, pBack, width_ * height_ * (bpp_ / 8));
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
CAVideoDevice::getSurface(CSurface ** surface, ESurfaceType type, bool bDouble)
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
