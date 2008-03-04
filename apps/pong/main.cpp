#include "bwm/bwm.h"
#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "stdlib.h"


#define REG_KEYS   (*(volatile unsigned short *)(0x04000130))
#define KEY_A      (1<<0)
#define KEY_B      (1<<1)
#define KEY_SELECT (1<<2)
#define KEY_START  (1<<3)
#define KEY_RIGHT  (1<<4)
#define KEY_LEFT   (1<<5)
#define KEY_UP     (1<<6)
#define KEY_DOWN   (1<<7)
#define KEY_R      (1<<8)
#define KEY_L      (1<<9)


// -----------------------------------------------------------------------------
void
runGame(CAVideoDevice * device, I2DRenderer * renderer, CSurface * surface)
{
  if((device == NULL) || (renderer == NULL) || (surface == NULL))
    return;

  renderer->setSurface(surface);

  uint32_t iPlayerWidth    = surface->width()  >> 5;
  uint32_t iPlayerHeight   = surface->height() >> 3;
  uint32_t iBallWidth      = surface->width()  >> 4;
  uint32_t iBallHeight     = surface->height() >> 5;
  uint32_t iTopWallEdge    = 20 + 4;
  uint32_t iRightWallEdge  = surface->width() - 4;
  uint32_t iBottomWallEdge = surface->height() - 4;
  uint32_t iPlayer1Edge    = 10 + iPlayerWidth;

  uint32_t iP1Pos = iTopWallEdge + 1;
  float fBallX = 0;
  float fBallY = iTopWallEdge;
  float fIncX = 2.12345f;
  float fIncY = 2.12345f;
  bool bMissed = false;
  bool bMissFlash = false;

  // Wait for vertical sync before drawing
  device->waitVSync();

  // Gray background
  renderer->setColor(64, 64, 64);
  renderer->fill();

  // Red frame
  renderer->setColor(255, 0, 0);
  renderer->fillRect(0, 20, surface->width(), 4);
  renderer->fillRect(0, surface->height() - 4, surface->width(), 4);
  renderer->fillRect(surface->width() - 4, 20 + 4, 4, surface->height() - 8);

  while(true)
  {
    // Update player 1
    if(~REG_KEYS & KEY_UP)
      iP1Pos -= 2;
    if(~REG_KEYS & KEY_DOWN)
      iP1Pos += 2;
    if(iP1Pos < iTopWallEdge)
      iP1Pos = iTopWallEdge;
    if((iP1Pos + iPlayerHeight) > iBottomWallEdge)
      iP1Pos = iBottomWallEdge - iPlayerHeight;

    // Update Ball
    fBallX += fIncX;
    fBallY += fIncY;

    if(fIncX > 0)
    {
      // Detect hit on right wall
      if((fBallX + iBallWidth - 1) >= iRightWallEdge)
      {
        fIncX = -fIncX;
        fBallX -= 2 * ((fBallX + iBallWidth - 1) - iRightWallEdge);
      }
    }
    else
    {
      // Detect hit on player 1
      if(fBallX < iPlayer1Edge)
      {
        if(bMissed == false)
        {
          if((fBallY < (iP1Pos + iPlayerHeight)) && ((fBallY + iBallHeight) > iP1Pos))
          {
            fIncX = -fIncX;
            fBallX += 2 * (iPlayer1Edge - fBallX);
          }
          else
          {
            bMissed = true;
          }
        }
        else
        {
          if(fBallX < 0)
          {
            fIncX = -fIncX;
            fBallX += 2 * (-fBallX);
            bMissed = false;
            bMissFlash = true;
          }
        }
      }
    }
    if(fIncY > 0)
    {
      // Detect hit on bottom wall
      if((fBallY + iBallHeight - 1) > iBottomWallEdge)
      {
        fIncY = -fIncY;
        fBallY -= 2 * ((fBallY + iBallHeight - 1) - iBottomWallEdge);
      }
    }
    else
    {
      // Detect hit on top wall
      if(fBallY < iTopWallEdge)
      {
        fIncY = -fIncY;
        fBallY += 2 * (iTopWallEdge - fBallY);
      }
    }

    // Wait for vertical sync before drawing
    device->waitVSync();

    // Clear play field
    if(bMissFlash == true)
    {
      bMissFlash = false;
      renderer->setColor(255, 0, 0);
    }
    else
      renderer->setColor(64, 64, 64);
    renderer->fillRect(0, iTopWallEdge, surface->width() - 4, surface->height() - iTopWallEdge - 4);
    //renderer->fill();

    // Draw player 1
    renderer->setColor(0, 255, 0);
    renderer->fillRect(10, iP1Pos, iPlayerWidth, iPlayerHeight);
    //renderer->fillRect(10, 4, iPlayerWidth, surface->height() - 8);

    // Draw ball
    renderer->setColor(0, 0, 255);
    renderer->fillRect((uint32_t)fBallX, (uint32_t)fBallY, iBallWidth, iBallHeight);

    // Flush everything to screen
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
        devices[iDev]->setMode(&modes[0]);
        CSurface    * pVideoSurface;
        I2DRenderer * pVideoRenderer;

        devices[iDev]->getSurface(&pVideoSurface, stSCREEN);
        devices[iDev]->displaySurface(pVideoSurface);
        devices[iDev]->getRenderer(&pVideoRenderer);
        runGame(devices[iDev], pVideoRenderer, pVideoSurface);

        if(pVideoRenderer != NULL)
          delete pVideoRenderer;
        if(pVideoSurface != NULL)
          delete pVideoSurface;
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
