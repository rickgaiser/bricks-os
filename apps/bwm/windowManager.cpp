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


#include "windowManager.h"
#include "desktop.h"
#include "event.h"
#include "eventKey.h"
#include "eventMouse.h"
#include "eventRedraw.h"
#include "stddef.h"


extern const unsigned short mouse[];
CAVideoDevice * pDisplayDevice;
CSurface      * pDisplaySurface;


namespace bwm
{


using namespace twl;


CWindowManager   windowManager;


//---------------------------------------------------------------------------
CWindowManager::CWindowManager()
 : pActiveWindow_(0)
 , bCursorActive_(true)
 , xOff_(0)
 , yOff_(0)
 , pDevice_(NULL)
 , pRenderer_(NULL)
 , pSurface_(NULL)
{
}

//---------------------------------------------------------------------------
CWindowManager::~CWindowManager()
{
}

//---------------------------------------------------------------------------
int
CWindowManager::init(CAVideoDevice * device)
{
  pDevice_ = device;

  if(pDevice_ != NULL)
  {
    const SVideoMode * mode;
    pDevice_->getDefaultMode(&mode);
    pDevice_->getSurface(&pSurface_, mode->width, mode->height);
    pDevice_->get2DRenderer(&pRenderer_);

    pDevice_->displaySurface(pSurface_);
    pRenderer_->setSurface(pSurface_);
    pDisplayDevice  = pDevice_;
    pDisplaySurface = pSurface_;
  }
  else
    return -1;

  return 0;
}

//---------------------------------------------------------------------------
CWindow *
CWindowManager::requestNewWindow()
{
  CWindow * pWindow = new CWindow(pRenderer_);

  pWindows_.push_back(pWindow);
  setActive(pWindow);

  return pWindow;
}

//---------------------------------------------------------------------------
int
CWindowManager::exec()
{
  CDesktop desktop;

  desktop.frame(false); // Desktop has no window frame
  desktop.rect(0, 0, pSurface_->mode.width, pSurface_->mode.height);
  desktop.visible(true); // Show desktop

  while(true)
  {
    // Wait for vertical sync
    pDevice_->waitVSync();

    // Redraw every window that needs to be redrawn
    for(uint32_t i(0); i < pWindows_.size(); i++)
      pWindows_[i]->event(CEventRedraw());

    // Flush all 2D operations (if not already done)
    pRenderer_->flush();
  }

  return 0;
}

//---------------------------------------------------------------------------
void
CWindowManager::setActive(CWindow * window)
{
  if(pActiveWindow_ != 0)
    pActiveWindow_->focus(false);

  pActiveWindow_ = window;
  pActiveWindow_->focus(true);
}

//---------------------------------------------------------------------------
void
CWindowManager::drawCursor()
{
/*
  int yoff;
  int iWidth (pSurface_->mode.width);
  int iHeight(pSurface_->mode.height);
  color_t clKey(0x7fff);

  for(int y(0); y < 16; y++)
  {
    if((y + yOff_) < iHeight)
    {
      yoff = y * 16;

      for(int x(0); x < 16; x++)
      {
        if((x + xOff_) < iWidth)
        {
          if(mouse[yoff + x] != clKey)
          {
            // Save what is underneath the cursor
            underCursor_[yoff + x] = pFB->pixel(x + xOff_, y + yOff_);
            // Draw cursor
            pFB->pixel(x + xOff_, y + yOff_) = mouse[yoff + x];
          }
        }
      }
    }
  }
*/
}

//---------------------------------------------------------------------------
void
CWindowManager::undrawCursor()
{
/*
  int yoff;
  int iWidth (pSurface_->mode.width);
  int iHeight(pSurface_->mode.height);
  color_t clKey(0x7fff);

  for(int y(0); y < 16; y++)
  {
    if((y + yOff_) < iHeight)
    {
      yoff = y * 16;

      for(int x(0); x < 16; x++)
      {
        if((x + xOff_) < iWidth)
        {
          if(mouse[yoff + x] != clKey)
          {
            // Restore what is underneath the cursor
            pFB->pixel(x + xOff_, y + yOff_) = underCursor_[yoff + x];
          }
        }
      }
    }
  }
*/
}


};
