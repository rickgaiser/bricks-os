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


#include "bwm/bwm.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"
#include "stdlib.h"


// -----------------------------------------------------------------------------
void
test2d(CAVideoDevice * device, I2DRenderer * renderer, CSurface * surface)
{
  int loops(256*5);

  if((renderer == NULL) || (surface == NULL))
    return;

  renderer->setSurface(surface);
/*
  // Irritating interlaced test
  for(uint32_t i(0); i < surface->height(); i++)
  {
    if(i < (1 * (surface->height() / 4)))
    {
      // 1st part
      if((i & 1) == 1)
      {
        // Red line
        renderer->setColor(255, 0, 0);
        renderer->drawHLine(0, i, surface->width());
      }
    }
    else if(i < (2 * (surface->height() / 4)))
    {
      // 2nd part
      if((i & 1) == 0)
      {
        // Green line
        renderer->setColor(0, 255, 0);
        renderer->drawHLine(0, i, surface->width());
      }
    }
    else if(i < (3 * (surface->height() / 4)))
    {
      // 3rd part
      if((i & 1) == 1)
      {
        // Blue line
        renderer->setColor(0, 0, 255);
        renderer->drawHLine(0, i, surface->width());
      }
    }
    else
    {
      // last part
      if((i & 1) == 0)
      {
        // White line
        renderer->setColor(255, 255, 255);
        renderer->drawHLine(0, i, surface->width());
      }
    }
  }
  renderer->flush();
  for(int i(0); i < loops; i++)
    device->waitVSync();
*/

  if(device->hasPositioning() == true)
  {
    int16_t maxH = device->getMaxHorizontalOffset();
    int16_t maxV = device->getMaxVerticalOffset();

    // Create window to easily detect screen edges
    // 2 Pixel white border
    renderer->setColor(255, 255, 255);
    renderer->fill();
    // 8 Pixel red border
    renderer->setColor(255, 0, 0);
    renderer->fillRect(2, 2, surface->width() - 4, surface->height() - 4);
    // Black filling
    renderer->setColor(0, 0, 0);
    renderer->fillRect(10, 10, surface->width() - 20, surface->height() - 20);
    renderer->flush();

    // Initial position 0, 0
    device->setHorizontalOffset(0);
    device->setVerticalOffset(0);

    //while(true)
    {
      // Move right (1,0 -> max,0)
      for(int16_t iX(1); iX < maxH; iX++)
      {
        device->setHorizontalOffset(iX);
        device->waitVSync();
      }
      // Move down (max,1 -> max,max)
      for(int16_t iY(1); iY < maxV; iY++)
      {
        device->setVerticalOffset(iY);
        device->waitVSync();
      }
      // Move left (max-1,max -> 0,max)
      for(int16_t iX(maxH - 1); iX >= 0; iX--)
      {
        device->setHorizontalOffset(iX);
        device->waitVSync();
      }
      // Move up (0,max-1 -> 0,0)
      for(int16_t iY(maxV - 1); iY >= 0; iY--)
      {
        device->setVerticalOffset(iY);
        device->waitVSync();
      }
    }

    // Center?
    device->setHorizontalOffset(maxH>>1);
    device->setVerticalOffset(maxV>>1);
  }

  // Full screen fill test
  for(int i(0); i < loops; i++)
  {
    renderer->setColor(i%256, 0, 0);
    renderer->fill();

    // Display progress bar
    renderer->setColor(255, 255, 255);
    renderer->fillRect(surface->width() / 4 + 0, surface->height() / 2 + 0,   surface->width() / 2,                   10 - 0);
    renderer->setColor(0, 0, 0);
    renderer->fillRect(surface->width() / 4 + 2, surface->height() / 2 + 2, ((surface->width() / 2 - 4) * i) / loops, 10 - 4);

    renderer->flush();
    device->waitVSync();
  }

  // Pixel test
  renderer->setColor(0, 0, 0);
  renderer->fill();
  for(int i(0); i < loops; i++)
  {
    renderer->setColor(rand() % 256, rand() % 256, rand() % 256);
    renderer->setPixel(
      rand() % surface->width(),
      rand() % surface->height());

    renderer->flush();
    device->waitVSync();
  }

  // Line test
  renderer->setColor(0, 0, 0);
  renderer->fill();
  for(int i(0); i < loops; i++)
  {
    renderer->setColor(rand() % 256, rand() % 256, rand() % 256);
    renderer->drawLine(
      rand() % surface->width(),
      rand() % surface->height(),
      rand() % surface->width(),
      rand() % surface->height());

    renderer->flush();
    device->waitVSync();
  }

  // Rect test
  renderer->setColor(0, 0, 0);
  renderer->fill();
  for(int i(0); i < loops; i++)
  {
    int x1 = rand() % surface->width();
    int y1 = rand() % surface->height();
    int x2 = rand() % surface->width();
    int y2 = rand() % surface->height();
    int x  = x1 < x2 ? x1 : x2;
    int y  = y1 < y2 ? y1 : y2;
    int w  = x1 < x2 ? (x2-x1) : (x1-x2);
    int h  = y1 < y2 ? (y2-y1) : (y1-y2);

    renderer->setColor(rand() % 256, rand() % 256, rand() % 256);
    renderer->drawRect(x, y, w, h);

    renderer->flush();
    device->waitVSync();
  }

  // Filled Rect test
  renderer->setColor(0, 0, 0);
  renderer->fill();
  for(int i(0); i < loops; i++)
  {
    int x1 = rand() % surface->width();
    int y1 = rand() % surface->height();
    int x2 = rand() % surface->width();
    int y2 = rand() % surface->height();
    int x  = x1 < x2 ? x1 : x2;
    int y  = y1 < y2 ? y1 : y2;
    int w  = x1 < x2 ? (x2-x1) : (x1-x2);
    int h  = y1 < y2 ? (y2-y1) : (y1-y2);

    renderer->setColor(rand() % 256, rand() % 256, rand() % 256);
    renderer->fillRect(x, y, w, h);

    renderer->flush();
    device->waitVSync();
  }
}

// -----------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  CAVideoDevice ** devices;
  int iDeviceCount;
  videoManager.listDevices(&devices, &iDeviceCount);
  if(iDeviceCount > 0)
  {
    for(int iDev(0); iDev < iDeviceCount; iDev++)
    {
      const SVideoMode * mode;
      devices[iDev]->getDefaultMode(&mode);
      devices[iDev]->setMode(mode);
      CSurface    * pVideoSurface;
      I2DRenderer * pVideoRenderer;

      // Test 2d with a single buffered surface
      devices[iDev]->getSurface(&pVideoSurface, mode->width, mode->height);
      devices[iDev]->displaySurface(pVideoSurface);
      devices[iDev]->get2DRenderer(&pVideoRenderer);
      test2d(devices[iDev], pVideoRenderer, pVideoSurface);

      if(pVideoRenderer != NULL)
        delete pVideoRenderer;
      if(pVideoSurface != NULL)
        delete pVideoSurface;
    }
  }
  else
  {
    printk("ERROR: No video devices!\n");
  }

  return 0;
}
