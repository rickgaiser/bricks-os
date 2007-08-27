#include "windowManager.h"
#include "event.h"
#include "eventKey.h"
#include "eventMouse.h"
#include "eventRedraw.h"
#include "frameBuffer.h"


extern const unsigned short mouse[];


namespace twl
{


CWindowManager   windowManager;


//---------------------------------------------------------------------------
CWindowManager::CWindowManager()
 : pActiveWindow_(0)
 , bCursorActive_(true)
 , xOff_(0)
 , yOff_(0)
{
}

//---------------------------------------------------------------------------
CWindowManager::~CWindowManager()
{
}

//---------------------------------------------------------------------------
CWindow *
CWindowManager::requestNewWindow()
{
  CWindow * pWindow = new CWindow;

  pWindows_.push_back(pWindow);
  setActive(pWindow);

  return pWindow;
}

//---------------------------------------------------------------------------
int
CWindowManager::exec()
{
  while(true)
  {
/*
    for(std::vector<CWindow *>::iterator itr(pWindows_.begin()); itr != pWindows_.end(); itr++)
      (*itr)->event(CEventRedraw());
*/
    if(bCursorActive_ == true)
      drawCursor();

    CWindow::swap(true);

    if(bCursorActive_ == true)
      undrawCursor();
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
  int yoff;
  int iWidth(240), iHeight(160);  // FIXME!
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
}

//---------------------------------------------------------------------------
void
CWindowManager::undrawCursor()
{
  int yoff;
  int iWidth(240), iHeight(160);  // FIXME!
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
}


};
