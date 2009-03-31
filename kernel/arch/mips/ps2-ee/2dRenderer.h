/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2009 Maximus32 <Maximus32@bricks-os.org>
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


#ifndef PS2_2DRENDERER_H
#define PS2_2DRENDERER_H


#include "kernel/2dRenderer.h"
#include "videoDevice.h"


//---------------------------------------------------------------------------
class CPS22DRenderer
 : public I2DRenderer
 , public CAPS2Renderer
{
public:
  CPS22DRenderer(CSurface * surf = 0);
  virtual ~CPS22DRenderer();

  // Surfaces
  virtual void       setSurface(CSurface * surface);
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

  // Clipping
  virtual void       setClipRect(int x, int y, unsigned int width, unsigned int height);

protected:
  struct SClipRect
  {
    int xmin;
    int ymin;
    int xmax;
    int ymax;
  };

private:
  // Current drawing color
  uint8_t  colorR_;
  uint8_t  colorG_;
  uint8_t  colorB_;
  uint64_t colorPS2_;

  // Clip Rect
  SClipRect clipper_;
};


#endif
