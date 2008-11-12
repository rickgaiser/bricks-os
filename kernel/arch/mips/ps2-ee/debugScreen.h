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


#ifndef ARCH_DEBUGSCREEN_H
#define ARCH_DEBUGSCREEN_H


#include "videoDevice.h"
#include "kernel/videoManager.h"
#include "kernel/fs.h"


#define TEXT_WIDTH 45
#define TEXT_HEIGHT 17


//---------------------------------------------------------------------------
class CPS2DebugScreen
 : public IFileIO
{
public:
  CPS2DebugScreen();
  virtual ~CPS2DebugScreen();

  virtual int init();

  // Inherited from IFileIO
  virtual ssize_t  write(const void * buffer, size_t size, bool block = false);

private:
  void cls();
  void setMode(SPS2VideoMode * mode);
  void printBegin();
  void printLine(uint16_t x, uint16_t y, char * str);
  void printEnd();

  SPS2VideoMode * pCurrentPS2Mode_;
  uint16_t actualHeight_;
  uint32_t freeMemAddr_;
  uint32_t frameAddr_[2];

  // Text
  unsigned int iCurrentX_;
  unsigned int iCurrentY_;
  char pBuffer_[TEXT_HEIGHT][TEXT_WIDTH];

  // Font
  uint32_t   g2_fontbuf_addr;    // address of font buffer
  uint32_t   g2_fontbuf_w;       // font buffer width
  uint32_t   g2_fontbuf_h;       // font buffer height
  uint16_t * g2_font_tc;         // pointer to texture coordinate buffer

  // GIF packet for DMA transmission to GS
  CGIFPacket packet_;
};


#endif
