#include "window.h"


namespace bwm
{


using namespace twl;


bool bTiny(false);


//---------------------------------------------------------------------------
CWindow::CWindow(I2DRenderer * renderer)
 : pEventHandler_(0)
 , cWindowRect_(0,  0, 2, 11)
 , cClientRect_(1, 10, 0,  0)
 , bNeedRedraw_(true)
 , bFocus_(true)
 , bFrame_(true)
 , pRenderer_(renderer)
{
  cWindowOnScreenRect_ = cWindowRect_;
//  cWindowOnScreenRect_.clip(pFB->rect());

  cClientOnScreenRect_ = cClientRect_;
//  cClientOnScreenRect_.clip(pFB->rect());
}

//---------------------------------------------------------------------------
CWindow::~CWindow()
{
}

//---------------------------------------------------------------------------
bool
CWindow::event(const CEvent & event)
{
  if((bFrame_ == true) && (bNeedRedraw_ == true) && (event.type() == CEvent::etRedraw))
  {
    color_t windowColor(bFocus_ ? clActiveWindow : clInActiveWindow);

    bNeedRedraw_ = false;

    if(bTiny == false)
    {
      // Light/Shades (2 pixel border)
      pRenderer_->setColor(clPanelFill); // left/top outer
      pRenderer_->drawVLine(cWindowRect_.x()    , cWindowRect_.y()    , cWindowRect_.height() - 1);
      pRenderer_->drawHLine(cWindowRect_.x() + 1, cWindowRect_.y()    , cWindowRect_.width()  - 2);
      pRenderer_->setColor(clPanelLight); // left/top inner
      pRenderer_->drawVLine(cWindowRect_.x() + 1, cWindowRect_.y() + 1, cWindowRect_.height() - 2);
      pRenderer_->drawHLine(cWindowRect_.x() + 2, cWindowRect_.y() + 1, cWindowRect_.width()  - 3);
      pRenderer_->setColor(clPanelShade1); // right/bottom outer
      pRenderer_->drawVLine(cWindowRect_.right() - 1, cWindowRect_.top()       , cWindowRect_.height()    );
      pRenderer_->drawHLine(cWindowRect_.left()     , cWindowRect_.bottom() - 1, cWindowRect_.width()  - 1);
      pRenderer_->setColor(clPanelShade2); // right/bottom inner
      pRenderer_->drawVLine(cWindowRect_.right() - 2, cWindowRect_.top()    + 1, cWindowRect_.height() - 2);
      pRenderer_->drawHLine(cWindowRect_.left()  + 1, cWindowRect_.bottom() - 2, cWindowRect_.width()  - 3);

      // Fill entire window
      pRenderer_->setColor(clPanelFill);
      pRenderer_->fillRect(cWindowRect_.x() + 2, cWindowRect_.y() + 2, cWindowRect_.width() - 4, cWindowRect_.height() - 4);

      // Place top bar
      pRenderer_->setColor(windowColor);
      pRenderer_->fillRect(cWindowRect_.x() + 4, cWindowRect_.y() + 4, cWindowRect_.width() - 8, 18);
    }
    else
    {
      // Fill entire window
      pRenderer_->setColor(clPanelFill);
      pRenderer_->fillRect(cWindowRect_.x(), cWindowRect_.y(), cWindowRect_.width(), cWindowRect_.height());
      // Place top bar
      pRenderer_->setColor(windowColor);
      pRenderer_->fillRect(cWindowRect_.x() + 2, cWindowRect_.y() + 2, cWindowRect_.width() - 4, 9);
    }
  }

  if(pEventHandler_ != 0)
    pEventHandler_->event(event);

  return true;
}

//---------------------------------------------------------------------------
void
CWindow::setEventHandler(IWindowEventHandler * eventHandler)
{
  pEventHandler_ = eventHandler;
}

//---------------------------------------------------------------------------
int
CWindow::width()
{
  return cClientRect_.width();
}

//---------------------------------------------------------------------------
int
CWindow::height()
{
  return cClientRect_.height();
}

//---------------------------------------------------------------------------
void
CWindow::rect(const CRect & rect)
{
  if(bTiny == false)
    cWindowRect_.rect(rect.x() - 4, rect.y() - 24, rect.width() + 8, rect.height() + 28);
  else
    cWindowRect_.rect(rect.x() - 2, rect.y() - 12, rect.width() + 4, rect.height() + 14);

  cWindowOnScreenRect_ = cWindowRect_;
  //cWindowOnScreenRect_.clip(pFB->rect());

  cClientRect_ = rect;
  cClientOnScreenRect_ = cClientRect_;
  //cClientOnScreenRect_.clip(pFB->rect());
}

//---------------------------------------------------------------------------
void
CWindow::rect(int x, int y, int width, int height)
{
  rect(CRect(x, y, width, height));
}

//---------------------------------------------------------------------------
CRect
CWindow::rect()
{
  return cClientRect_;
}

//---------------------------------------------------------------------------
void
CWindow::focus(bool focus)
{
  bFocus_ = focus;
}

//---------------------------------------------------------------------------
bool
CWindow::focus() const
{
  return bFocus_;
}

//---------------------------------------------------------------------------
void
CWindow::frame(bool frame)
{
  bFrame_ = frame;
}

//---------------------------------------------------------------------------
bool
CWindow::frame() const
{
  return bFrame_;
}

//---------------------------------------------------------------------------
void
CWindow::setColor(color_t rgb)
{
  pRenderer_->setColor(rgb);
}

//---------------------------------------------------------------------------
void
CWindow::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  pRenderer_->setColor(r, g, b);
}

//---------------------------------------------------------------------------
void
CWindow::setPixel(int x, int y)
{
  pRenderer_->setPixel(x, y);
}

//---------------------------------------------------------------------------
void
CWindow::fill()
{
  pRenderer_->fillRect(cClientOnScreenRect_.left(), cClientOnScreenRect_.top(), cClientOnScreenRect_.width(), cClientOnScreenRect_.height());
}

//---------------------------------------------------------------------------
void
CWindow::fillRect(int x, int y, unsigned int width, unsigned int height)
{
  pRenderer_->fillRect(x, y, width, height);
}

//---------------------------------------------------------------------------
void
CWindow::drawLine(int x1, int y1, int x2, int y2)
{
  pRenderer_->drawLine(x1, y1, x2, y2);
}

//---------------------------------------------------------------------------
void
CWindow::drawHLine(int x, int y, unsigned int width)
{
  pRenderer_->drawHLine(x, y, width);
}

//---------------------------------------------------------------------------
void
CWindow::drawVLine(int x, int y, unsigned int height)
{
  pRenderer_->drawVLine(x, y, height);
}

//---------------------------------------------------------------------------
void
CWindow::drawRect(int x, int y, unsigned int width, unsigned int height)
{
  pRenderer_->drawRect(x, y, width, height);
}


};
