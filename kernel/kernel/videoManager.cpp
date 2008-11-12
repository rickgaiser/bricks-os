/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "kernel/videoManager.h"
#include "string.h"
#include "stdlib.h"


//---------------------------------------------------------------------------
CVideoManager videoManager;

const SColorFormatOperations colorFormatOps[] =
{
    {32, 8, 8, 8, 8, 0, 0, 0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000}  //  0, cfUNKNOWN
    // (A/X)RGB(A/X) (32bit)
  , {32, 0, 0, 0, 0,16, 8, 0,24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}  //  1, cfA8R8G8B8, ARGB
  , {32, 0, 0, 0, 0,16, 8, 0,24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}  //  2, cfX8R8G8B8, XRGB
  , {32, 0, 0, 0, 0,24,16, 8, 0, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff}  //  3, cfR8G8B8A8, RGBA
  , {32, 0, 0, 0, 0,24,16, 8, 0, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff}  //  4, cfR8G8B8X8, RGBX
    // (A/X)BGR(A/X) (32bit)
  , {32, 0, 0, 0, 0, 0, 8,16,24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000}  //  5, cfA8B8G8R8, ABGR
  , {32, 0, 0, 0, 0, 0, 8,16,24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000}  //  6, cfX8B8G8R8, XBGR
  , {32, 0, 0, 0, 0, 8,16,24, 0, 0xff000000, 0x000000ff, 0x0000ff00, 0x00ff0000}  //  7, cfB8G8R8A8, BGRA
  , {32, 0, 0, 0, 0, 8,16,24, 0, 0xff000000, 0x000000ff, 0x0000ff00, 0x00ff0000}  //  8, cfB8G8R8X8, BGRX
    // RGB/BGR (24bit)
  , {32, 0, 0, 0, 8,16, 8, 0, 0, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000}  //  9, cfR8G8B8,   RGB
  , {32, 0, 0, 0, 8, 0, 8,16, 0, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000}  // 10, cfB8G8R8,   BGR
    // (A/X)RGB(A/X) (16bit)
  , {16, 3, 2, 3, 8,11, 5, 0, 0, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000}  // 11, cfR5G6B5,   RGB
  , {16, 3, 3, 3, 7,10, 5, 0,15, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000}  // 12, cfA1R5G5B5, ARGB
  , {16, 3, 3, 3, 7,10, 5, 0,15, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000}  // 13, cfX1R5G5B5, XRGB
  , {16, 4, 4, 4, 4, 8, 4, 0,12, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000}  // 14, cfA4R4G4B4, ARGB
  , {16, 4, 4, 4, 4, 8, 4, 0,12, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000}  // 15, cfX4R4G4B4, XRGB
  , {16, 3, 3, 3, 7,11, 6, 1, 0, 0x0000f800, 0x000007c0, 0x0000003e, 0x00000001}  // 16, cfR5G5B5A1, RGBA
  , {16, 3, 3, 3, 7,11, 6, 1, 0, 0x0000f800, 0x000007c0, 0x0000003e, 0x00000001}  // 17, cfR5G5B5X1, RGBX
  , {16, 4, 4, 4, 4,12, 8, 4, 0, 0x0000f000, 0x00000f00, 0x000000f0, 0x0000000f}  // 18, cfR4G4B4A4, RGBA
  , {16, 4, 4, 4, 4,12, 8, 4, 0, 0x0000f000, 0x00000f00, 0x000000f0, 0x0000000f}  // 19, cfR4G4B4X4, RGBX
    // (A/X)BGR(A/X) (16bit)
  , {16, 3, 2, 3, 8, 0, 5,11, 0, 0x0000001f, 0x000007e0, 0x0000f800, 0x00000000}  // 20, cfB5G6R5,   BGR
  , {16, 3, 3, 3, 7, 0, 5,10,15, 0x0000001f, 0x000003e0, 0x00007c00, 0x00008000}  // 21, cfA1B5G5R5, ABGR
  , {16, 3, 3, 3, 7, 0, 5,10,15, 0x0000001f, 0x000003e0, 0x00007c00, 0x00008000}  // 22, cfX1B5G5R5, XBGR
  , {16, 4, 4, 4, 4, 0, 4, 8,12, 0x0000000f, 0x000000f0, 0x00000f00, 0x0000f000}  // 23, cfA4B4G4R4, ABGR
  , {16, 4, 4, 4, 4, 0, 4, 8,12, 0x0000000f, 0x000000f0, 0x00000f00, 0x0000f000}  // 24, cfX4B4G4R4, XBGR
  , {16, 3, 3, 3, 7, 1, 6,11, 0, 0x0000003e, 0x000007c0, 0x0000f800, 0x00000001}  // 25, cfB5G5R5A1, BGRA
  , {16, 3, 3, 3, 7, 0, 5,10,15, 0x0000003e, 0x000007c0, 0x0000f800, 0x00000001}  // 26, cfB5G5R5X1, BGRX
  , {16, 4, 4, 4, 4, 4, 8,12, 0, 0x000000f0, 0x00000f00, 0x0000f000, 0x0000000f}  // 27, cfB4G4R4A4, BGRA
  , {16, 4, 4, 4, 4, 4, 8,12, 0, 0x000000f0, 0x00000f00, 0x0000f000, 0x0000000f}  // 28, cfB4G4R4X4, BGRX
    // Palettized RGB (8bit)
  , { 8, 5, 5, 6, 8, 5, 2, 0, 0, 0x000000e0, 0x0000001c, 0x00000003, 0x00000000}  // 29, cfR3G3B2,   RGB
    // Palettized
  , { 8, 8, 8, 8, 8, 0, 0, 0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000}  // 30, cfP8
  , { 4, 8, 8, 8, 8, 0, 0, 0, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000}  // 31, cfP4
};


//---------------------------------------------------------------------------
void
BxColorFormat_CreateConverter(SColorFormatConverter & conv, EColorFormat from, EColorFormat to)
{
  const SColorFormatOperations & fmtFrom = colorFormatOps[from];
  const SColorFormatOperations & fmtTo   = colorFormatOps[to];

  // Mask takes out the source color
  conv.maskR  = fmtFrom.maskR;
  conv.maskG  = fmtFrom.maskG;
  conv.maskB  = fmtFrom.maskB;
  conv.maskA  = fmtFrom.maskA;

  // Shift right selected color
  conv.rshiftR = fmtFrom.shiftR;
  conv.rshiftG = fmtFrom.shiftG;
  conv.rshiftB = fmtFrom.shiftB;
  conv.rshiftA = fmtFrom.shiftA;

  // Shift left into destination position
  conv.lshiftR = fmtTo.shiftR;
  conv.lshiftG = fmtTo.shiftG;
  conv.lshiftB = fmtTo.shiftB;
  conv.lshiftA = fmtTo.shiftA;

  // Destination has more loss:
  //  - Shift right the extra loss
  if(fmtTo.lossR > fmtFrom.lossR)
    conv.rshiftR += fmtTo.lossR - fmtFrom.lossR;
  if(fmtTo.lossG > fmtFrom.lossG)
    conv.rshiftG += fmtTo.lossG - fmtFrom.lossG;
  if(fmtTo.lossB > fmtFrom.lossB)
    conv.rshiftB += fmtTo.lossB - fmtFrom.lossB;
  if(fmtTo.lossA > fmtFrom.lossA)
    conv.rshiftA += fmtTo.lossA - fmtFrom.lossA;

  // Destination has more bits:
  //  - Shift left the extra bits
  if(fmtTo.lossR < fmtFrom.lossR)
    conv.lshiftR += fmtFrom.lossR - fmtTo.lossR;
  if(fmtTo.lossG < fmtFrom.lossG)
    conv.lshiftG += fmtFrom.lossG - fmtTo.lossG;
  if(fmtTo.lossB < fmtFrom.lossB)
    conv.lshiftB += fmtFrom.lossB - fmtTo.lossB;
  if(fmtTo.lossA < fmtFrom.lossA)
    conv.lshiftA += fmtFrom.lossA - fmtTo.lossA;
}

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
CAVideoDevice::bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy)
{
  uint8_t         bytespp(source->mode.bpp >> 3);
  const uint8_t * pSrc = &((uint8_t *)source->p)[(sy * source->mode.xpitch + sx) * bytespp];
  uint8_t       * pDst = &((uint8_t *)dest->p)  [(dy * dest->mode.xpitch   + dx) * bytespp];

  if(source->mode.format != dest->mode.format)
  {
    return;
    /*
    // Convert source to destination format
    for(int iY(0); iY < h; iY++)
    {
      for (int iX(0); iX < w; iX++)
      {
      }
    }
    */
  }

  // Copy, line by line
  for(int iY(0); iY < h; iY++)
  {
    memcpy(pDst, pSrc, w * bytespp);
    pSrc += source->mode.xpitch * bytespp;
    pDst += dest->mode.xpitch   * bytespp;
  }
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
