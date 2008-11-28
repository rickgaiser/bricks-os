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


bool bKeyUp   = false;
bool bKeyDown = false;
uint32_t iWallWidth = 1;

float fSpeed  = MIN_SPEED;
uint32_t iSpeedBarHeight = 1;
bool bRedrawSpeed = true;
bool bSpeedFlash = false;
float fSpeedFlash = 0.0f;
bool bSpeedIncrease = true;


// -----------------------------------------------------------------------------
void
updateSpeed(float diff)
{
  if(diff != 0)
  {
    // Calculate new speed
    float fNewSpeed = fSpeed + diff;
    if(fNewSpeed < MIN_SPEED) fNewSpeed = MIN_SPEED;
    if(fNewSpeed > MAX_SPEED) fNewSpeed = MAX_SPEED;

    // Notify if changed
    if(fNewSpeed != fSpeed)
    {
      fSpeed = fNewSpeed;
      bRedrawSpeed = true;
      bSpeedIncrease = (diff > 0);
    }
  }
}

// -----------------------------------------------------------------------------
void
drawSpeed(I2DRenderer * renderer, CSurface * surface)
{
  if(bSpeedFlash == true)
  {
    float fFlash = 1.0f;

    // Flash the speedbar
    if(fSpeedFlash >= 1.0f)
    {
      // Reset the speedflash
      fSpeedFlash = 0.0f;
      bSpeedFlash = false;
    }
    else
    {
      fFlash *= fSpeedFlash;
      fSpeedFlash += 0.1f;
    }

    // Set speed bar
    renderer->setColor(0, (uint8_t)(255 * fFlash), 0);
    renderer->fillRect(0, 0, surface->width(), iSpeedBarHeight);
  }
  else if(bRedrawSpeed == true)
  {
    bRedrawSpeed = false;

    if(bSpeedIncrease == false)
    {
      // Clear speed bar
      renderer->setColor(0, 0, 0);
      renderer->fillRect(0, 0, surface->width(), iSpeedBarHeight);
    }

    // Calculate the percentage of the speed
    float fSpeedPart = (fSpeed - MIN_SPEED) * (1.0f / (MAX_SPEED - MIN_SPEED));
    int32_t width = (int32_t)((float)surface->width() * fSpeedPart);

    // Draw (only if visible)
    if(width > 0)
    {
      // Set speed bar
      renderer->setColor((uint8_t)(255 * (1.0f - fSpeedPart)), (uint8_t)(255 * fSpeedPart), 0);
      renderer->fillRect(0, 0, width, iSpeedBarHeight);
    }
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
class CBox
{
public:
  CBox(float width, float height) : iWidth_(width), iHeight_(height) {}

  float x(){return iX_;}
  float y(){return iY_;}
  float top(){return iY_;}
  float bottom(){return iY_ + iHeight_;}
  float left(){return iX_;}
  float right(){return iX_ + iWidth_;}
  float width(){return iWidth_;}
  float height(){return iHeight_;}

  void setColor(uint8_t r, uint8_t g, uint8_t b){iR_ = r; iG_ = g; iB_ = b;}
  void setPos(float x, float y){iX_ = x; iY_ = y;}
  void x(float x){iX_ = x;}
  void y(float y){iY_ = y;}

  void draw(I2DRenderer * renderer)
  {
    renderer->setColor(iR_, iG_, iB_);
    renderer->fillRect((int32_t)iX_, (int32_t)iY_, (int32_t)iWidth_, (int32_t)iHeight_);
  }

private:
  float iWidth_;
  float iHeight_;
  float iX_;
  float iY_;
  uint8_t iR_;
  uint8_t iG_;
  uint8_t iB_;
};

// -----------------------------------------------------------------------------
void
runGame(CAVideoDevice * device, I2DRenderer * renderer, CSurface * surface, CAControllerDevice * controller)
{
  if((device == NULL) || (renderer == NULL) || (surface == NULL))
    return;

  renderer->setSurface(surface);

  CBox player1(surface->width() >> 5, surface->height() >> 3);
  CBox ball   (surface->width() >> 4, surface->height() >> 5);
  iSpeedBarHeight       = surface->height() / 10;
  iWallWidth            = surface->height() / 20;
  float fTopWallEdge    = iSpeedBarHeight   + iWallWidth;
  float fRightWallEdge  = surface->width()  - iWallWidth;
  float fBottomWallEdge = surface->height() - iWallWidth;

  float iP1Pos = fTopWallEdge + ((fBottomWallEdge - fTopWallEdge) / 2) - (player1.height() / 4);
  player1.setPos(10, iP1Pos);
  player1.setColor(0, 255, 0);

  ball.setPos(0, fTopWallEdge);
  ball.setColor(0, 255, 0);

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
  drawSpeed(renderer, surface);
  // Red frame
  renderer->setColor(255, 0, 0);
  renderer->fillRect(0, iSpeedBarHeight, surface->width(), iWallWidth);
  renderer->fillRect(0, surface->height() - iWallWidth, surface->width(), iWallWidth);
  renderer->fillRect(surface->width() - iWallWidth, iSpeedBarHeight + iWallWidth, iWallWidth, surface->height() - 2 * iWallWidth);

  while(true)
  {
    getInput(controller);

    // Update Player1 Position
    if(bKeyUp == true)
      player1.y(player1.y() - 2);
    if(bKeyDown == true)
      player1.y(player1.y() + 2);
    if(player1.y() < fTopWallEdge)
      player1.y(fTopWallEdge);
    if((player1.bottom()) > fBottomWallEdge)
      player1.y(fBottomWallEdge - player1.height());

    // Update Ball Position
    ball.x(ball.x() + fIncX * fSpeed);
    ball.y(ball.y() + fIncY * fSpeed);

    // Detect hit on player1
    if((fIncX < 0) && (ball.left() < player1.right()))
    {
      if(bMissed == false)
      {
        if((ball.bottom() > player1.top()) && (ball.top() < player1.bottom()))
        {
          // Reverse direction
          fIncX = -fIncX;
          // Correct overshoot
          ball.x(ball.x() + 2.0f * (player1.right() - ball.left()));

          // It's a hit!
          iGreenFlash = FLASH_FRAME_COUNT;
          updateSpeed(SPEED_UP);

          if(fSpeed == MAX_SPEED)
            bSpeedFlash = true;
        }
        else
        {
          bMissed = true;
        }
      }
    }

    // Detect hit on walls
    if(fIncX > 0)
    {
      // Detect hit on right wall
      if(ball.right() > fRightWallEdge)
      {
        // Reverse direction
        fIncX = -fIncX;
        // Correct overshoot
        ball.x(ball.x() - 2.0f * (ball.right() - fRightWallEdge));
      }
    }
    else
    {
      if(ball.left() < 0)
      {
        // Reverse direction
        fIncX = -fIncX;
        // Correct overshoot
        ball.x(ball.x() + 2.0f * (0 - ball.left()));

        // We realle missed if we hit this wall...
        bMissed = false;
        iRedFlash = FLASH_FRAME_COUNT;
        updateSpeed(-SPEED_DOWN);
      }
    }
    if(fIncY > 0)
    {
      // Detect hit on bottom wall
      if(ball.bottom() > fBottomWallEdge)
      {
        // Reverse direction
        fIncY = -fIncY;
        // Correct overshoot
        ball.y(ball.y() - 2.0f * (ball.bottom() - fBottomWallEdge));
      }
    }
    else
    {
      // Detect hit on top wall
      if(ball.top() < fTopWallEdge)
      {
        // Reverse direction
        fIncY = -fIncY;
        // Correct overshoot
        ball.y(ball.y() + 2.0f * (fTopWallEdge - ball.top()));
      }
    }

    // Wait for vertical sync before drawing
    device->waitVSync();

    // Draw speedbar
    drawSpeed(renderer, surface);

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
    renderer->fillRect(0, (int32_t)fTopWallEdge, surface->width() - iWallWidth, surface->height() - (int32_t)fTopWallEdge - iWallWidth);

    // Draw player 1
    player1.draw(renderer);

    // Draw ball
    ball.draw(renderer);

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
