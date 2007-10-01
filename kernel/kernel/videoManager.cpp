#include "kernel/videoManager.h"
#include "string.h"
#include "stdlib.h"


CVideoManager videoManager;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CSurface::CSurface()
 : p(0)
 , pFront(0)
 , pBack(0)
 , color_(BxRGB(0, 0, 0))
 , fillColor_(BxRGB(0, 0, 0))
{
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
CSurface::setColor(color_t color)
{
  color_ = color;
}

//---------------------------------------------------------------------------
color_t
CSurface::getColor()
{
  return color_;
}

//---------------------------------------------------------------------------
void
CSurface::setFillColor(color_t color)
{
  fillColor_ = color;
}

//---------------------------------------------------------------------------
color_t
CSurface::getFillColor()
{
  return fillColor_;
}

//---------------------------------------------------------------------------
void
CSurface::setPixel(int x, int y)
{
  ((uint16_t *)p)[y * width_ + x] = color_;
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
  int iLineOffset(y * width_ + x);

  for(int iY(y); iY < (y + height); iY++)
  {
    for(int iX(0); iX < width; iX++)
      ((uint16_t *)p)[iLineOffset + iX] = fillColor_;
    iLineOffset += width_;
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
        ((uint16_t *)p)[((int)(currenty) * width_) + x] = color_;
        currenty += slopey;
      }
    }
    else
    {
      for(int x(x2); x <= x1; x++)
      {
        ((uint16_t *)p)[((int)(currenty) * width_) + x] = color_;
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
        ((uint16_t *)p)[(y * width_) + (int)currentx] = color_;
        currentx += slopex;
      }
    }
    else
    {
      for(int y(y2); y <= y1; y++)
      {
        ((uint16_t *)p)[(y * width_) + (int)currentx] = color_;
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
CSurface::swap(bool bForceCopy)
{
  // "bForceCopy" can be ignored, we always copy
  if(pBack != 0)
    memcpy(pFront, pBack, width_ * height_ * sizeof(pixel_t));
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
