#include "window.h"


namespace twl
{


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

    pRenderer_->setColor(clPanelFill);
    pRenderer_->fillRect(cWindowRect_.x(), cWindowRect_.y(), cWindowRect_.width(), 10);
    pRenderer_->drawVLine(cWindowRect_.x(),  cWindowRect_.y() + 10, cWindowRect_.height() - 10);
    pRenderer_->drawVLine(cWindowRect_.right() - 1, cWindowRect_.y() + 10, cWindowRect_.height() - 10);
    pRenderer_->drawHLine(cWindowRect_.x() + 1, cWindowRect_.bottom() - 1, cWindowRect_.width() - 2);
    pRenderer_->setColor(windowColor);
    pRenderer_->fillRect(cWindowRect_.x() + 1, cWindowRect_.y() + 1, cWindowRect_.width() - 2, 8);
    //pRenderer_->setColor(clWhite);
    //pRenderer_->drawText(cWindowRect_.left() + 2, cWindowRect_.top() + 2, "Window");
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
  cWindowRect_.rect(rect.x() - 1, rect.y() - 10, rect.width() + 2, rect.height() + 11);
  cWindowOnScreenRect_ = cWindowRect_;
//  cWindowOnScreenRect_.clip(pFB->rect());

  cClientRect_ = rect;
  cClientOnScreenRect_ = cClientRect_;
//  cClientOnScreenRect_.clip(pFB->rect());
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
CWindow::fill(color_t color)
{
  pRenderer_->setColor(color);
  pRenderer_->fillRect(cClientOnScreenRect_.left(), cClientOnScreenRect_.top(), cClientOnScreenRect_.width(), cClientOnScreenRect_.height());
}

//---------------------------------------------------------------------------
void
CWindow::fillRect(const CRect & rect, color_t color)
{
  CRect fillRect(rect);
  fillRect.clip(cClientOnScreenRect_);

  if(fillRect.valid() == true)
  {
    pRenderer_->setColor(color);
    pRenderer_->fillRect(fillRect.x(), fillRect.y(), fillRect.width(), fillRect.height());
  }
}

//---------------------------------------------------------------------------
void
CWindow::drawHLine(int x, int y, int width, color_t color)
{
  // Clip left
//  if(left < cClientOnScreenRect_.left())
//    left = cClientOnScreenRect_.left();
  // Clip right
//  if(right > cClientOnScreenRect_.right())
//    right = cClientOnScreenRect_.right();

  // Check y and validate line
//  if((right > left) && (cClientOnScreenRect_.contains(left, y) == true))
  {
    pRenderer_->setColor(color);
    pRenderer_->drawHLine(x, y, width);
  }
}

//---------------------------------------------------------------------------
void
CWindow::drawVLine(int x, int y, int height, color_t color)
{
  // Clip top
//  if(top < cClientOnScreenRect_.top())
//    top = cClientOnScreenRect_.top();
  // Clip bottom
//  if(bottom > cClientOnScreenRect_.bottom())
//    bottom = cClientOnScreenRect_.bottom();

  // Check x and validate line
//  if((top < bottom) && (cClientOnScreenRect_.contains(x, top) == true))
  {
    pRenderer_->setColor(color);
    pRenderer_->drawVLine(x, y, height);
  }
}

//---------------------------------------------------------------------------
void
CWindow::drawText(int x, int y, const char * string, color_t color)
{
//  pRenderer_->setColor(color);
//  pRenderer_->drawText(x, y, string);
}


};
