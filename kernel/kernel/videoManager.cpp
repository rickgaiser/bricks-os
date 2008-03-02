#include "kernel/videoManager.h"
#include "string.h"
#include "stdlib.h"


//---------------------------------------------------------------------------
#define SET_PIXEL(x,y,c) \
{ \
  switch(pSurface_->mode.bpp) \
  { \
    case 8: \
      ((uint8_t  *)pSurface_->p)[y * pSurface_->mode.xpitch + x] = c; \
      break; \
    case 16: \
      ((uint16_t *)pSurface_->p)[y * pSurface_->mode.xpitch + x] = c; \
      break; \
    case 32: \
      ((uint32_t *)pSurface_->p)[y * pSurface_->mode.xpitch + x] = c; \
      break; \
    default: \
      ; \
  }; \
}

//---------------------------------------------------------------------------
#define VISIBLE_POINT(x,y) ((x >= 0) && (x < (int)pSurface_->mode.width) && (y >= 0) && (y < (int)pSurface_->mode.height))

//---------------------------------------------------------------------------
#define VISIBLE_RECT(x,y,w,h) ((x < (int)pSurface_->mode.width) && ((x + w - 1) >= 0) && (y < (int)pSurface_->mode.height) && ((y + h - 1) >= 0))

//---------------------------------------------------------------------------
#define CLIP_POINT(x,y) \
{ \
  if(x < 0) x = 0; \
  if(y < 0) y = 0; \
  else if((unsigned int)x >= pSurface_->mode.width)  x = pSurface_->mode.width - 1; \
  else if((unsigned int)y >= pSurface_->mode.height) y = pSurface_->mode.height -1; \
}

//---------------------------------------------------------------------------
// Make sure the rect is visible first, using 'VISIBLE(x,y,w,h);'
// Otherwise this macro makes no sense
#define CLIP_RECT(x,y,w,h) \
{ \
  if(x < 0) {w -= -x; x = 0;} \
  if(y < 0) {h -= -y; y = 0;} \
  if((x + w) > pSurface_->mode.width)  {w = pSurface_->mode.width  - x;} \
  if((y + h) > pSurface_->mode.height) {h = pSurface_->mode.height - y;} \
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
{
}

//---------------------------------------------------------------------------
CSurface::~CSurface()
{
}

//---------------------------------------------------------------------------
uint32_t
CSurface::xpitch()
{
  return mode.xpitch;
}

//---------------------------------------------------------------------------
uint32_t
CSurface::ypitch()
{
  return mode.ypitch;
}

//---------------------------------------------------------------------------
uint32_t
CSurface::width()
{
  return mode.width;
}

//---------------------------------------------------------------------------
uint32_t
CSurface::height()
{
  return mode.height;
}

//---------------------------------------------------------------------------
uint32_t
CSurface::bpp()
{
  return mode.bpp;
}

//---------------------------------------------------------------------------
EColorFormat
CSurface::format()
{
  return mode.format;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
C2DRenderer::C2DRenderer(CSurface * surf)
 : pSurface_(surf)
{
  color_.r = 0;
  color_.g = 0;
  color_.b = 0;
  color_.a = 255;

  if(pSurface_ != NULL)
    fmtColor_ = BxColorFormat_FromRGB(pSurface_->mode.format, color_.r, color_.g, color_.b);
}

//---------------------------------------------------------------------------
C2DRenderer::~C2DRenderer()
{
}

//---------------------------------------------------------------------------
void
C2DRenderer::setSurface(CSurface * surface)
{
  pSurface_ = surface;

  if(pSurface_ != NULL)
    fmtColor_ = BxColorFormat_FromRGB(pSurface_->mode.format, color_.r, color_.g, color_.b);
}

//---------------------------------------------------------------------------
CSurface *
C2DRenderer::getSurface()
{
  return pSurface_;
}

//---------------------------------------------------------------------------
void
C2DRenderer::flush()
{
}

//---------------------------------------------------------------------------
void
C2DRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  color_.r = r;
  color_.g = g;
  color_.b = b;
  color_.a = 255;

  if(pSurface_ != NULL)
    fmtColor_ = BxColorFormat_FromRGB(pSurface_->mode.format, color_.r, color_.g, color_.b);
}

//---------------------------------------------------------------------------
void
C2DRenderer::setPixel(int x, int y)
{
  if(pSurface_ != NULL)
  {
    if(VISIBLE_POINT(x, y))
      setPixel_i(x, y);
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::fill()
{
  if(pSurface_ != NULL)
  {
    fill_i();
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  if(pSurface_ != NULL)
  {
    if(VISIBLE_RECT(x, y, width, height))
    {
      CLIP_RECT(x, y, width, height);
      fillRect_i(x, y, width, height);
    }
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawLine(int x1, int y1, int x2, int y2)
{
  if(pSurface_ != NULL)
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
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  if(pSurface_ != NULL)
  {
    if(VISIBLE_RECT(x, y, width, height))
    {
      CLIP_RECT(x, y, width, height);
      drawRect_i(x, y, width, height);
    }
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::setPixel_i(int x, int y)
{
  SET_PIXEL(x, y, fmtColor_);
}

//---------------------------------------------------------------------------
void
C2DRenderer::fill_i()
{
  fillRect_i(0, 0, pSurface_->mode.width, pSurface_->mode.height);
}

//---------------------------------------------------------------------------
void
C2DRenderer::fillRect_i(int x, int y, unsigned int width, unsigned int height)
{
  for(unsigned int iY(y); iY < (y + height); iY++)
  {
    for(unsigned int iX(x); iX < (x + width); iX++)
      SET_PIXEL(iX, iY, fmtColor_);
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawLine_i(int x1, int y1, int x2, int y2)
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
C2DRenderer::drawHLine_i(int x, int y, unsigned int width)
{
  while(width > 0)
  {
    SET_PIXEL(x, y, fmtColor_);
    x++;
    width--;
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawVLine_i(int x, int y, unsigned int height)
{
  while(height > 0)
  {
    SET_PIXEL(x, y, fmtColor_);
    y++;
    height--;
  }
}

//---------------------------------------------------------------------------
void
C2DRenderer::drawRect_i(int x, int y, unsigned int width, unsigned int height)
{
  if((width == 0) || (height == 0))
  {
    return;
  }
  else if(width == 1)
  {
    drawVLine_i(x, y, height);
  }
  else if(width == 2)
  {
    drawVLine_i(x,     y, height);
    drawVLine_i(x + 1, y, height);
  }
  else if(height == 1)
  {
    drawHLine_i(x, y, width);
  }
  else if(height == 2)
  {
    drawHLine_i(x, y,     width);
    drawHLine_i(x, y + 1, width);
  }
  else
  {
    drawHLine_i(x, y, width);                      // Top
    drawHLine_i(x, y + height - 1, width);         // Bottom
    drawVLine_i(x, y + 1, height - 2);             // Left
    drawVLine_i(x + width - 1, y + 1, height - 2); // Right
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAVideoDevice::CAVideoDevice()
 : vSync_(true)
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
CAVideoDevice::setVSync(bool vsync)
{
  vSync_ = vsync;
}

//---------------------------------------------------------------------------
bool
CAVideoDevice::getVSync()
{
  return vSync_;
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
