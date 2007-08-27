#include "windowImpl.h"
#include "windowManager.h"
#include "window.h"


namespace twl
{


class CWindowImpl::CPrivateData
{
public:
  CWindow * pWindow_;
};


//---------------------------------------------------------------------------
CWindowImpl::CWindowImpl()
 : pPrivateData_(new CPrivateData)
{
  pPrivateData_->pWindow_ = windowManager.requestNewWindow();
}

//---------------------------------------------------------------------------
CWindowImpl::CWindowImpl(int width, int height)
 : pPrivateData_(new CPrivateData)
{
  pPrivateData_->pWindow_ = windowManager.requestNewWindow();
  pPrivateData_->pWindow_->rect(0, 0, width, height);
}

//---------------------------------------------------------------------------
CWindowImpl::~CWindowImpl()
{
}

//---------------------------------------------------------------------------
void
CWindowImpl::setEventHandler(IWindowEventHandler * eventHandler)
{
  pPrivateData_->pWindow_->setEventHandler(eventHandler);
}

//---------------------------------------------------------------------------
void
CWindowImpl::rect(const CRect & rect)
{
  pPrivateData_->pWindow_->rect(rect);
}

//---------------------------------------------------------------------------
void
CWindowImpl::rect(int left, int top, int width, int height)
{
  pPrivateData_->pWindow_->rect(left, top, width, height);
}

//---------------------------------------------------------------------------
CRect
CWindowImpl::rect()
{
  return pPrivateData_->pWindow_->rect();
}

//---------------------------------------------------------------------------
void
CWindowImpl::frame(bool frame)
{
  if(pPrivateData_->pWindow_ != 0)
    pPrivateData_->pWindow_->frame(frame);
}

//---------------------------------------------------------------------------
bool
CWindowImpl::frame() const
{
  bool bRetVal(false);

  if(pPrivateData_->pWindow_ != 0)
    bRetVal = pPrivateData_->pWindow_->frame();

  return bRetVal;
}

//---------------------------------------------------------------------------
pixel_t &
CWindowImpl::pixel(int x, int y)
{
  return pPrivateData_->pWindow_->pixel(x, y);
}

//---------------------------------------------------------------------------
void
CWindowImpl::fill(color_t color)
{
  pPrivateData_->pWindow_->fill(color);
}

//---------------------------------------------------------------------------
void
CWindowImpl::fillRect(const CRect & rect, color_t color)
{
  pPrivateData_->pWindow_->fillRect(rect, color);
}

//---------------------------------------------------------------------------
void
CWindowImpl::drawHLine(int y, int left, int right, color_t color)
{
  pPrivateData_->pWindow_->drawHLine(y, left, right, color);
}

//---------------------------------------------------------------------------
void
CWindowImpl::drawVLine(int x, int top, int bottom, color_t color)
{
  pPrivateData_->pWindow_->drawVLine(x, top, bottom, color);
}

//---------------------------------------------------------------------------
void
CWindowImpl::drawText(int x, int y, const char * string, color_t color)
{
  pPrivateData_->pWindow_->drawText(x, y, string, color);
}

//---------------------------------------------------------------------------
void
CWindowImpl::swap()
{
  pPrivateData_->pWindow_->swap();
}


};
