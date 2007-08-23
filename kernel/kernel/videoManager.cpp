#include "kernel/videoManager.h"
#include "string.h"


CVideoManager videoManager;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CSurface::CSurface()
 : p(0)
 , pFront(0)
 , pBack(0)
{
}

//---------------------------------------------------------------------------
CSurface::~CSurface()
{
}

//---------------------------------------------------------------------------
void
CSurface::fill(color_t color)
{
  fillRect(0, 0, width, height, color);
}

//---------------------------------------------------------------------------
void
CSurface::fillRect(int x, int y, int width, int height, color_t color)
{
  for(int iY(y); iY < (y + height); iY++)
  {
    int iLineOffset(iY * this->width + x);

    for(int iX(0); iX < width; iX++)
    {
      ((uint16_t *)p)[iLineOffset + iX] = color;
    }
  }
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
    memcpy(pFront, pBack, width * height * sizeof(pixel_t));
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