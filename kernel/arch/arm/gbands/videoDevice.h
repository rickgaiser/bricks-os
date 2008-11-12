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


#ifndef GBANDS_VIDEODEVICE_H
#define GBANDS_VIDEODEVICE_H


#include "kernel/2dRenderer.h"
#include "kernel/interruptManager.h"
#include "kernel/pthread_k.h"
#include "kernel/videoManager.h"
#include "asm/arch/memory.h"
#include "asm/arch/config.h"


//---------------------------------------------------------------------------
class CGBA2DRenderer
 : public C2DRenderer
{
public:
  CGBA2DRenderer();
  virtual ~CGBA2DRenderer();

protected:
  virtual void fill_i();
  virtual void fillRect_i(int x, int y, unsigned int width, unsigned int height);
  virtual void drawHLine_i(int x, int y, unsigned int width);
};

//---------------------------------------------------------------------------
class CGBAVideoDevice
 : public CAVideoDevice
 , public IInterruptServiceRoutine
{
public:
  CGBAVideoDevice();
  virtual ~CGBAVideoDevice();

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

private:
  // Surface we're currently displaying
  CSurface * pSurface_;
  uint8_t iSurfacesFree_;
  vuint32_t iFrameCount_; // volatile, becouse the isr updates it

#ifdef CONFIG_MULTI_THREADING
  pthread_mutex_t mutex_;       // The locking mutex
  pthread_cond_t condVSync_;    // State change condition
#else
  volatile bool bSwap_; // volatile, becouse the isr updates it
#endif

  const SVideoMode * pCurrentMode_;
};


#endif
