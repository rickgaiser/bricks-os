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


#ifndef BWM_WINDOW_H
#define BWM_WINDOW_H


#include "event.h"
#include "rect.h"
#include "color.h"
#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"
#include <stdint.h>


namespace bwm
{


enum EBltType
{
   eBT_COPY  = 0
  ,eBT_MERGE = 1
};

class CWindow
 : public I2DRenderer
{
public:
  CWindow(I2DRenderer * renderer);
  virtual ~CWindow();

  // Incomming events from OS
  bool event(const twl::CEvent & event);

  // Set application event handler
  void setEventHandler(twl::IWindowEventHandler * eventHandler);

  // Geometry functions
  int width();
  int height();
  void rect(const twl::CRect & rect);
  void rect(int x, int y, int width, int height);
  twl::CRect rect();

  // Window functions
  void focus(bool focus);
  bool focus() const;
  void frame(bool focus);
  bool frame() const;

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
  // Window
  twl::IWindowEventHandler * pEventHandler_;
  twl::CRect cWindowRect_;
  twl::CRect cWindowOnScreenRect_;
  // Client area
  twl::CRect cClientRect_;
  twl::CRect cClientOnScreenRect_;

  bool bNeedRedraw_;
  bool bFocus_;
  bool bFrame_;

  // Display surface
  I2DRenderer * pRenderer_;
};


};


#endif
