#include "window.h"


namespace twl
{


//---------------------------------------------------------------------------
CWindow::CWindow()
 : pEventHandler_(0)
 , cWindowRect_(0,  0, 2, 11)
 , cClientRect_(1, 10, 0,  0)
 , bFocus_(true)
 , bFrame_(true)
{
  cWindowOnScreenRect_ = cWindowRect_;
//  cWindowOnScreenRect_.clip(pFB->rect());

  cClientOnScreenRect_ = cClientRect_;
//  cClientOnScreenRect_.clip(pFB->rect());
}

//---------------------------------------------------------------------------
CWindow::CWindow(int width, int height)
 : pEventHandler_(0)
 , cWindowRect_(0,  0, width + 2, height + 11)
 , cClientRect_(1, 10, width,     height)
 , bFocus_(true)
 , bFrame_(true)
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
  if((bFrame_ == true) && (event.type() == CEvent::etRedraw))
  {
    //color_t windowColor(bFocus_ ? clActiveWindow : clInActiveWindow);

    //pRenderer_->setColor(clPanelFill);
    pRenderer_->fillRect(cWindowRect_.left(), cWindowRect_.top(), cWindowRect_.width(), 10);
    pRenderer_->drawVLine(cWindowRect_.left(),  cWindowRect_.top() + 10, cWindowRect_.bottom());
    pRenderer_->drawVLine(cWindowRect_.right(), cWindowRect_.top() + 10, cWindowRect_.bottom());
    pRenderer_->drawHLine(cWindowRect_.bottom(), cWindowRect_.left(), cWindowRect_.right());
    //pRenderer_->setColor(windowColor);
    pRenderer_->fillRect(cWindowRect_.left() + 1, cWindowRect_.top() + 1, cWindowRect_.width() - 2, 8);
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
  cWindowRect_.rect(rect.left() - 1, rect.top() - 10, rect.width() + 2, rect.height() + 11);
  cWindowOnScreenRect_ = cWindowRect_;
//  cWindowOnScreenRect_.clip(pFB->rect());

  cClientRect_ = rect;
  cClientOnScreenRect_ = cClientRect_;
//  cClientOnScreenRect_.clip(pFB->rect());
}

//---------------------------------------------------------------------------
void
CWindow::rect(int left, int top, int width, int height)
{
  rect(CRect(left, top, width, height));
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
pixel_t
CWindow::pixel(int x, int y)
{
  return 0;//pRenderer_->pixel(x, y);
}

//---------------------------------------------------------------------------
void
CWindow::fill(color_t color)
{
  //pRenderer_->setColor(color);
  pRenderer_->fillRect(cClientOnScreenRect_.left(), cClientOnScreenRect_.top(), cClientOnScreenRect_.width(), cClientOnScreenRect_.height());
}

//---------------------------------------------------------------------------
void
CWindow::fillRect(const CRect & rect, color_t color)
{
  CRect fillRect(rect);
  fillRect.clip(cClientOnScreenRect_);

  //pRenderer_->setColor(color);
  if(fillRect.valid() == true)
    pRenderer_->fillRect(fillRect.left(), fillRect.top(), fillRect.width(), fillRect.height());
}

//---------------------------------------------------------------------------
void
CWindow::drawHLine(int y, int left, int right, color_t color)
{
  // Clip left
  if(left < cClientOnScreenRect_.left())
    left = cClientOnScreenRect_.left();
  // Clip right
  if(right > cClientOnScreenRect_.right())
    right = cClientOnScreenRect_.right();

  //pRenderer_->setColor(color);
  // Check y and validate line
  if((right > left) && (cClientOnScreenRect_.contains(left, y) == true))
    pRenderer_->drawHLine(y, left, right);
}

//---------------------------------------------------------------------------
void
CWindow::drawVLine(int x, int top, int bottom, color_t color)
{
  // Clip top
  if(top < cClientOnScreenRect_.top())
    top = cClientOnScreenRect_.top();
  // Clip bottom
  if(bottom > cClientOnScreenRect_.bottom())
    bottom = cClientOnScreenRect_.bottom();

  //pRenderer_->setColor(color);
  // Check x and validate line
  if((top < bottom) && (cClientOnScreenRect_.contains(x, top) == true))
    pRenderer_->drawVLine(x, top, bottom);
}

//---------------------------------------------------------------------------
void
CWindow::drawText(int x, int y, const char * string, color_t color)
{
//  pRenderer_->setColor(color);
//  pRenderer_->drawText(x, y, string);
}


};
