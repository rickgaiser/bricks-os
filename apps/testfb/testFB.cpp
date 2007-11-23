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
test2d(CSurface * surface)
{
/*
  for(int i(0); i < 0x001f; i++)
  {
    // Fill entire screen with one color
    surface->setFillColor(i);
    surface->fill();

    // Display progress bar
    surface->setFillColor(clWhite);
    surface->fillRect(2, surface->height() - 12, surface->width() - 4, 10);
    surface->setFillColor(clBlack);
    surface->fillRect(3, surface->height() - 10, ((surface->width() - 6) * i) / 0x001f, 6);

    surface->waitVSync();
  }
*/

  // Pixel test
  surface->setFillColor(0, 0, 0);
  surface->fill();
  for(int i(0); i < 0x00FF; i++)
  {
    surface->setColor(rand() % 255, rand() % 255, rand() % 255);
    surface->setPixel(
      rand() % surface->width(),
      rand() % surface->height());

    surface->waitVSync();
  }

  // Line test
  surface->setFillColor(0, 0, 0);
  surface->fill();
  for(int i(0); i < 0x00FF; i++)
  {
    surface->setColor(rand() % 255, rand() % 255, rand() % 255);
    surface->drawLine(
      rand() % surface->width(),
      rand() % surface->height(),
      rand() % surface->width(),
      rand() % surface->height());

    surface->waitVSync();
  }

  // Rect test
  surface->setFillColor(0, 0, 0);
  surface->fill();
  for(int i(0); i < 0x00FF; i++)
  {
    int x1 = rand() % surface->width();
    int y1 = rand() % surface->height();
    int x2 = rand() % surface->width();
    int y2 = rand() % surface->height();
    int x  = x1 < x2 ? x1 : x2;
    int y  = y1 < y2 ? y1 : y2;
    int w  = x1 < x2 ? (x2-x1) : (x1-x2);
    int h  = y1 < y2 ? (y2-y1) : (y1-y2);

    surface->setColor(rand() % 255, rand() % 255, rand() % 255);
    surface->drawRect(x, y, w, h);

    surface->waitVSync();
  }

  // Filled Rect test
  surface->setFillColor(0, 0, 0);
  surface->fill();
  for(int i(0); i < 0x00FF; i++)
  {
    int x1 = rand() % surface->width();
    int y1 = rand() % surface->height();
    int x2 = rand() % surface->width();
    int y2 = rand() % surface->height();
    int x  = x1 < x2 ? x1 : x2;
    int y  = y1 < y2 ? y1 : y2;
    int w  = x1 < x2 ? (x2-x1) : (x1-x2);
    int h  = y1 < y2 ? (y2-y1) : (y1-y2);

    surface->setFillColor(rand() % 255, rand() % 255, rand() % 255);
    surface->fillRect(x, y, w, h);

    surface->waitVSync();
  }
}

// -----------------------------------------------------------------------------
int
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
          CSurface * pVideoSurface;

          // Test 2d with a single buffered surface
          devices[iDev]->getSurface(&pVideoSurface, stSCREEN, false);
          test2d(pVideoSurface);
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
