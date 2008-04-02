#include "bwm/bwm.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "stdlib.h"


// -----------------------------------------------------------------------------
void
test2d(CAVideoDevice * device, I2DRenderer * renderer, CSurface * surface)
{
  if((renderer == NULL) || (surface == NULL))
    return;

  renderer->setSurface(surface);

  // Full screen fill test
  int loops = 256*5;
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
      const SVideoMode * modes;
      int iModeCount;
      devices[iDev]->listModes(&modes, &iModeCount);
      if(iModeCount > 0)
      {
        for(int iMode(0); iMode < iModeCount; iMode++)
        {
          devices[iDev]->setMode(&modes[iMode]);
          CSurface    * pVideoSurface;
          I2DRenderer * pVideoRenderer;

          // Test 2d with a single buffered surface
          devices[iDev]->getSurface(&pVideoSurface, modes[iMode].width, modes[iMode].height);
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
        printk("ERROR: Device has no modes!\n");
      }
    }
  }
  else
  {
    printk("ERROR: No video devices!\n");
  }

  return 0;
}
