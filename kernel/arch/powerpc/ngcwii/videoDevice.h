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


#ifndef ARCH_VIDEODEVICE_H
#define ARCH_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"
#include "kernel/3dRenderer.h"
#include "../../../../gl/rasterScanline.h"


//---------------------------------------------------------------------------
class CNGCVideoDevice;

//---------------------------------------------------------------------------
class CNGC2DRenderer
 : public C2DRenderer
{
public:
  CNGC2DRenderer(CNGCVideoDevice * dev);
  virtual ~CNGC2DRenderer();

  // Flush operations to surface
  virtual void flush();

private:
  CNGCVideoDevice * pDev_;
};

//-----------------------------------------------------------------------------
class CNGCSoftRasterizer
: public raster::CRasterizerScanline
{
public:
  CNGCSoftRasterizer(CNGCVideoDevice * dev);
  virtual ~CNGCSoftRasterizer();

  // Flush operations to surface
  virtual void flush();

private:
  CNGCVideoDevice * pDev_;
};

//---------------------------------------------------------------------------
class CNGCVideoDevice
 : public CAVideoDevice
{
public:
  CNGCVideoDevice();
  virtual ~CNGCVideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount);
  virtual void getCurrentMode(const SVideoMode ** mode);
  virtual void getDefaultMode(const SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, int width, int height);
  virtual void get2DRenderer(I2DRenderer ** renderer);
  virtual void get3DRenderer(I3DRenderer ** renderer);

  virtual uint32_t getFrameNr(); // Returns current frame/field number
  virtual uint32_t waitVSync(); // Returns current frame/field number, after vsync
  virtual void displaySurface(CSurface * surface);

  // Flush RGB buffer to native screen
  void flush(CSurface * surface);

  void setHorizontal(uint16_t xoffset, uint16_t width);
  void setVertical  (uint16_t yoffset, uint16_t height);
  void setInterlaced(bool interlaced);

  // Screen positioning (in pixels)
  virtual bool     hasPositioning();
  virtual uint16_t getMaxHorizontalOffset();
  virtual uint16_t getHorizontalOffset();
  virtual void     setHorizontalOffset(uint16_t x);
  virtual uint16_t getMaxVerticalOffset();
  virtual uint16_t getVerticalOffset();
  virtual void     setVerticalOffset(uint16_t y);

private:
  // Surface we're currently displaying
  CSurface * pSurface_;

  // Current Native surface (RGB mode only)
  uint8_t * pSurfaceData_;
  void * pNativeSurface_;

  const SVideoMode * pCurrentMode_;

  vuint32_t iFrameCount_; // volatile, becouse the isr updates it
  uint16_t iCurrentHOffset_;
  uint16_t iCurrentVOffset_;
};


#endif
