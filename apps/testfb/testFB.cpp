#include "bwm/bwm.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "stdlib.h"


/*
// Standard colors
color_t clBlack           (BxRGB(  0,   0,   0));
color_t clDGray           (BxRGB( 64,  64,  64));
color_t clGray            (BxRGB(128, 128, 128));
color_t clLGray           (BxRGB(192, 192, 192));
color_t clWhite           (BxRGB(255, 255, 255));
color_t clRed             (BxRGB(255,   0,   0));
color_t clGreen           (BxRGB(  0, 255,   0));
color_t clBlue            (BxRGB(  0,   0, 255));
// Default desktop color
color_t clDesktop         (BxRGB( 58, 110, 165));
// Window title-bar colors
color_t clActiveWindowL   (BxRGB( 10,  36, 106));
color_t clActiveWindowR   (BxRGB(166, 202, 240));
color_t clActiveWindow    (clActiveWindowL);
color_t clInActiveWindowL (BxRGB(128, 128, 128));
color_t clInActiveWindowR (BxRGB(192, 192, 192));
color_t clInActiveWindow  (clInActiveWindowL);
// Panel colors
color_t clPanelLight      (BxRGB(255, 255, 255));
color_t clPanelShade1     (BxRGB( 64,  64,  64));
color_t clPanelShade2     (BxRGB(128, 128, 128));
color_t clPanelFill       (BxRGB(212, 208, 200));
*/


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
    renderer->setColor(i%255, 0, 0);
    renderer->fill();

    // Display progress bar
    renderer->setColor(255, 255, 255);
    renderer->fillRect(surface->width() / 4 + 0, surface->height() / 2 + 0,   surface->width() / 2,                   10 - 0);
    renderer->setColor(0, 0, 0);
    renderer->fillRect(surface->width() / 4 + 2, surface->height() / 2 + 2, ((surface->width() / 2 - 4) * i) / loops, 10 - 4);

    device->waitVSync();
    renderer->flush();
  }

  // Pixel test
  renderer->setColor(0, 0, 0);
  renderer->fill();
  for(int i(0); i < loops; i++)
  {
    renderer->setColor(rand() % 255, rand() % 255, rand() % 255);
    renderer->setPixel(
      rand() % surface->width(),
      rand() % surface->height());

    device->waitVSync();
    renderer->flush();
  }

  // Line test
  renderer->setColor(0, 0, 0);
  renderer->fill();
  for(int i(0); i < loops; i++)
  {
    renderer->setColor(rand() % 255, rand() % 255, rand() % 255);
    renderer->drawLine(
      rand() % surface->width(),
      rand() % surface->height(),
      rand() % surface->width(),
      rand() % surface->height());

    device->waitVSync();
    renderer->flush();
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

    renderer->setColor(rand() % 255, rand() % 255, rand() % 255);
    renderer->drawRect(x, y, w, h);

    device->waitVSync();
    renderer->flush();
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

    renderer->setColor(rand() % 255, rand() % 255, rand() % 255);
    renderer->fillRect(x, y, w, h);

    device->waitVSync();
    renderer->flush();
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
          devices[iDev]->getSurface(&pVideoSurface, stSCREEN);
          devices[iDev]->displaySurface(pVideoSurface);
          devices[iDev]->getRenderer(&pVideoRenderer);
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
