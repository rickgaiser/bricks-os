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
#include "kernel/inputManager.h"
#include "kernel/2dRenderer.h"
#include "stdlib.h"


#define FLASH_FRAME_COUNT 5
#define MIN_SPEED         2.0f
#define MAX_SPEED         6.0f
#define SPEED_UP          0.2f
#define SPEED_DOWN        0.6f


float fSpeed  = MIN_SPEED;
bool bKeyUp   = false;
bool bKeyDown = false;


// -----------------------------------------------------------------------------
void
updateSpeed(I2DRenderer * renderer, CSurface * surface, float diff)
{
  fSpeed += diff;

  if(fSpeed < MIN_SPEED) fSpeed = MIN_SPEED;
  if(fSpeed > MAX_SPEED) fSpeed = MAX_SPEED;

  // Clear speed bar
  renderer->setColor(0, 0, 0);
  renderer->fillRect(0, 0, surface->width(), 20);

  float fSpeedPart = (fSpeed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED);
  if(fSpeedPart > 0.0f)
  {
    // Set speed bar
    renderer->setColor((uint8_t)(255 * (1.0f - fSpeedPart)), (uint8_t)(255 * fSpeedPart), 0);
    renderer->fillRect(0, 0, (uint32_t)(surface->width() * fSpeedPart), 20);
  }
}

// -----------------------------------------------------------------------------
void
getInput(CAControllerDevice * controller)
{
  if(controller != NULL)
  {
    uint32_t keys = controller->getButtonState();
    bKeyUp    = (keys & CTRL_BTN_UP);
    bKeyDown  = (keys & CTRL_BTN_DOWN);
  }
}

// -----------------------------------------------------------------------------
void
runGame(CAVideoDevice * device, I2DRenderer * renderer, CSurface * surface, CAControllerDevice * controller)
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

  uint32_t iP1Pos = iTopWallEdge + ((iBottomWallEdge - iTopWallEdge) >> 1) - (iPlayerHeight >> 2);
  float fBallX = 0;
  float fBallY = iTopWallEdge;
  float fIncX = 1.0f;
  float fIncY = 0.5f;
  bool  bMissed = false;
  int   iRedFlash = 0;
  int   iGreenFlash = 0;

  // Wait for vertical sync before drawing
  device->waitVSync();

  // Gray background
  renderer->setColor(0, 0, 0);
  renderer->fill();

  // Speed bar
  updateSpeed(renderer, surface, 0.0f);
  // Red frame
  renderer->setColor(255, 0, 0);
  renderer->fillRect(0, 20, surface->width(), 4);
  renderer->fillRect(0, surface->height() - 4, surface->width(), 4);
  renderer->fillRect(surface->width() - 4, 20 + 4, 4, surface->height() - 8);

  while(true)
  {
    getInput(controller);

    // Update player 1
    if(bKeyUp == true)
      iP1Pos -= 2;
    if(bKeyDown == true)
      iP1Pos += 2;
    if(iP1Pos < iTopWallEdge)
      iP1Pos = iTopWallEdge;
    if((iP1Pos + iPlayerHeight) > iBottomWallEdge)
      iP1Pos = iBottomWallEdge - iPlayerHeight;

    // Update Ball
    fBallX += fIncX * fSpeed;
    fBallY += fIncY * fSpeed;

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
            /*
            // Calculate new ball Y speed
            int32_t iP1Center   = iP1Pos + (iPlayerHeight >> 1);
            int32_t iBallCenter = (int32_t)fBallY + (iBallHeight   >> 1);
            float fOffset = iBallCenter - iP1Center;
            fOffset /= (iPlayerHeight >> 1);  // nomalize to -1..1
            fOffset *= 2.0f;

            fIncY += fOffset;
            if(fIncY >  2.0f) fIncY =  2.0f;
            if(fIncY < -2.0f) fIncY = -2.0f;
            */

            fIncX = -fIncX;
            fBallX += 2 * (iPlayer1Edge - fBallX);

            iGreenFlash = FLASH_FRAME_COUNT;
            updateSpeed(renderer, surface, SPEED_UP);
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
            bMissed = false;

            fIncX = -fIncX;
            fBallX += 2 * (-fBallX);

            iRedFlash = FLASH_FRAME_COUNT;
            updateSpeed(renderer, surface, -SPEED_DOWN);
          }
        }
      }
    }
    if(fIncY > 0)
    {
      // Detect hit on bottom wall
      if((fBallY + iBallHeight) > iBottomWallEdge)
      {
        fIncY = -fIncY;
        fBallY -= 2 * ((fBallY + iBallHeight) - iBottomWallEdge);
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
    if(iRedFlash > 0)
    {
      bMissed = false;
      renderer->setColor((255 * iRedFlash) / FLASH_FRAME_COUNT, 0, 0);
      iRedFlash--;
    }
    else if(iGreenFlash > 0)
    {
      bMissed = false;
      renderer->setColor(0, (255 * iGreenFlash) / FLASH_FRAME_COUNT, 0);
      iGreenFlash--;
    }
    else
      renderer->setColor(0, 0, 0);
    renderer->fillRect(0, iTopWallEdge, surface->width() - 4, surface->height() - iTopWallEdge - 4);
    //renderer->fill();

    // Draw player 1
    renderer->setColor(0, 255, 0);
    renderer->fillRect(10, iP1Pos, iPlayerWidth, iPlayerHeight);
    //renderer->fillRect(10, 4, iPlayerWidth, surface->height() - 8);

    // Draw ball
    renderer->setColor(0, 255, 0);
    renderer->fillRect((uint32_t)fBallX, (uint32_t)fBallY, iBallWidth, iBallHeight);

    // Flush everything to screen
    renderer->flush();
  }
}

// -----------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  CAVideoDevice ** videoDevices;
  int iVideoDeviceCount;
  CAInputDevice ** inputDevices;
  int iInputDeviceCount;

  CAVideoDevice      * pVideoDevice   = NULL;
  CSurface           * pVideoSurface  = NULL;
  I2DRenderer        * pVideoRenderer = NULL;
  CAControllerDevice * pController    = NULL;

  int iRetVal(0);

  videoManager.listDevices(&videoDevices, &iVideoDeviceCount);
  if(iVideoDeviceCount > 0)
  {
    const SVideoMode * pMode;

    pVideoDevice = videoDevices[0];
    pVideoDevice->getDefaultMode(&pMode);
    if(pMode != NULL)
    {
      // Set default video mode
      pVideoDevice->setMode(pMode);
      pVideoDevice->getSurface(&pVideoSurface, pMode->width, pMode->height);
      pVideoDevice->displaySurface(pVideoSurface);
      pVideoDevice->get2DRenderer(&pVideoRenderer);
    }
    else
    {
      printk("ERROR: Video device has no default mode!\n");
      iRetVal = -1;
    }
  }
  else
  {
    printk("ERROR: No video devices!\n");
    iRetVal = -1;
  }

  if(iRetVal == 0)
  {
    // Initialize input device
    inputManager.listDevices(&inputDevices, &iInputDeviceCount);
    for(int i(0); i < iInputDeviceCount; i++)
    {
      if(inputDevices[i]->type() == IDT_CONTROLLER)
      {
        pController = (CAControllerDevice *)inputDevices[i];
        break;
      }
    }

    // Start the game
    runGame(pVideoDevice, pVideoRenderer, pVideoSurface, pController);
  }

  // Release allocated memory
  if(pVideoRenderer != NULL)
    delete pVideoRenderer;
  if(pVideoSurface != NULL)
    delete pVideoSurface;
  if(pController != NULL)
    delete pController;

  return iRetVal;
}
