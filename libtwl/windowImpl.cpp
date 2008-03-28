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
CWindowImpl::rect(int x, int y, int width, int height)
{
  pPrivateData_->pWindow_->rect(x, y, width, height);
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
CWindowImpl::drawHLine(int x, int y, int width, color_t color)
{
  pPrivateData_->pWindow_->drawHLine(x, y, width, color);
}

//---------------------------------------------------------------------------
void
CWindowImpl::drawVLine(int x, int y, int height, color_t color)
{
  pPrivateData_->pWindow_->drawVLine(x, y, height, color);
}

//---------------------------------------------------------------------------
void
CWindowImpl::drawText(int x, int y, const char * string, color_t color)
{
  pPrivateData_->pWindow_->drawText(x, y, string, color);
}


};
