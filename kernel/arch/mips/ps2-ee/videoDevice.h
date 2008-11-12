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


#ifndef PS2_VIDEODEVICE_H
#define PS2_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"
#include "kernel/fs.h"
#include "kernel/interruptManager.h"
#include "asm/arch/memory.h"
#include "dma.h"
#include "gif.h"


//---------------------------------------------------------------------------
struct SPS2CRTCMode
{
  uint8_t biosMode;
  uint8_t vck;
};

//---------------------------------------------------------------------------
struct SPS2VideoMode
{
  uint16_t width;
  uint16_t height;
  uint16_t interlaced;
  const SPS2CRTCMode * crtcMode;
  uint16_t xoffset;
  uint16_t yoffset;
};

//---------------------------------------------------------------------------
class CPS2Surface
 : public CSurface
{
public:
  CPS2Surface();
  virtual ~CPS2Surface();

public:
  uint16_t psm_;
};

//---------------------------------------------------------------------------
class CAPS2Renderer
 : public virtual IRenderer
{
public:
  CAPS2Renderer(CSurface * surf = 0);
  ~CAPS2Renderer();

  // Surfaces
  virtual void       setSurface(CSurface * surface);
  virtual CSurface * getSurface();

  // Flush operations to surface
  virtual void       flush();

protected:
  // Surface we're currently rendering on
  CPS2Surface * pSurface_;

  // GIF packet for DMA transmission to GS
  CGIFPacket    packet_;

  // Data for DMA transfer to GS
  bool bDataWaiting_;
};

//---------------------------------------------------------------------------
class CPS22DRenderer
 : public I2DRenderer
 , public CAPS2Renderer
{
public:
  CPS22DRenderer(CSurface * surf = 0);
  virtual ~CPS22DRenderer();

  // Surfaces
  virtual void       setSurface(CSurface * surface){CAPS2Renderer::setSurface(surface);}
  virtual CSurface * getSurface()                  {return CAPS2Renderer::getSurface();}

  // Flush operations to surface
  virtual void       flush()                       {CAPS2Renderer::flush();}

  // Color
  virtual void       setColor(color_t rgb); // cfA8R8G8B8 format color
  virtual void       setColor(uint8_t r, uint8_t g, uint8_t b);

  // Drawing
  virtual void       setPixel(int x, int y);
  virtual void       fill();
  virtual void       fillRect(int x, int y, unsigned int width, unsigned int height);
  virtual void       drawLine(int x1, int y1, int x2, int y2);
  virtual void       drawHLine(int x, int y, unsigned int width);
  virtual void       drawVLine(int x, int y, unsigned int height);
  virtual void       drawRect(int x, int y, unsigned int width, unsigned int height);

private:
  // Current drawing color
  SColor color_;
};

//---------------------------------------------------------------------------
class CPS2VideoDevice
 : public CAVideoDevice
 , public IInterruptServiceRoutine
{
public:
  CPS2VideoDevice();
  virtual ~CPS2VideoDevice();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

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

  virtual void bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy);

  bool allocFramebuffer(uint32_t & addr, int w, int h, uint16_t psm);
  bool allocTexture(uint32_t & addr, int w, int h, uint16_t psm);

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
  CPS2Surface * pSurface_;

  // GIF packet for DMA transmission to GS
  CGIFPacket    packet_;

  const SVideoMode    * pCurrentMode_;
  const SPS2VideoMode * pCurrentPS2Mode_;
  uint32_t freeMemAddr_;
  uint16_t actualHeight_;
  uint16_t currentPSM_;
  uint16_t currentInterlaced_;
  uint16_t currentField_;
  uint16_t currentDoubleScan_;

  vuint32_t iFrameCount_; // volatile, becouse the isr updates it
  uint16_t iCurrentHOffset_;
  uint16_t iCurrentVOffset_;

#ifdef CONFIG_MULTI_THREADING
  pthread_mutex_t mutex_;       // The locking mutex
  pthread_cond_t condVSync_;    // State change condition
#else
  volatile bool bSwap_; // volatile, becouse the isr updates it
#endif
};


#endif
