#include "kernel/videoManager.h"
#include "string.h"
#include "stdlib.h"


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
    // Palettized RGB (8bit)
  , { 8, 5, 5, 6, 8, 5, 2, 0, 0, 0x000000e0, 0x0000001c, 0x00000003, 0x00000000}  // cfR3G3B2
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
CVideoManager::bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy)
{
  // FIXME: What device owns the surface?
  devices_[0]->bitBlt(dest, dx, dy, w, h, source, sx, sy);
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
