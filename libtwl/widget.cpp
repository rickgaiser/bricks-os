#include "widget.h"


#define FORALLWIDGETS   for(uint32_t iWidgetIdx(0); iWidgetIdx < pChildren_.size(); iWidgetIdx++)(pChildren_[iWidgetIdx])


namespace twl
{


//---------------------------------------------------------------------------
CWidget::CWidget(CWidget * parent)
 : pParent_(parent)
 , rectRelative_(0, 0, 0, 0)
 , rectAbsolute_(0, 0, 0, 0)
 , bNeedRedraw_(true)
{
  if(pParent_ != 0)
  {
    pWindowImpl_ = pParent_->pWindowImpl_;
    pParent_->insertChild(this);
  }
  else
  {
    pWindowImpl_ = new CWindowImpl;
    pWindowImpl_->setEventHandler(this);
  }
}

//---------------------------------------------------------------------------
CWidget::~CWidget()
{
}

//---------------------------------------------------------------------------
int
CWidget::left()
{
  return rectRelative_.left();
}

//---------------------------------------------------------------------------
int
CWidget::top()
{
  return rectRelative_.top();
}

//---------------------------------------------------------------------------
int
CWidget::width()
{
  return rectRelative_.width();
}

//---------------------------------------------------------------------------
int
CWidget::height()
{
  return rectRelative_.height();
}

//---------------------------------------------------------------------------
void
CWidget::rect(const CRect & rect)
{
  rectRelative_ = rect;
  rectAbsolute_ = rect;
  if(pParent_ != 0)
  {
    rectAbsolute_.x(rectAbsolute_.x() + pParent_->rectAbsolute().x());
    rectAbsolute_.y(rectAbsolute_.y() + pParent_->rectAbsolute().y());
  }
  else
  {
    pWindowImpl_->rect(rectAbsolute_);
  }
}

//---------------------------------------------------------------------------
void
CWidget::rect(int x, int y, int width, int height)
{
  this->rect(CRect(x, y, width, height));
}

//---------------------------------------------------------------------------
CRect
CWidget::rect()
{
  return rectRelative_;
}

//---------------------------------------------------------------------------
CRect
CWidget::rectClient()
{
  return rectRelative_;
}

//---------------------------------------------------------------------------
CRect
CWidget::rectAbsolute()
{
  return rectAbsolute_;
}

//---------------------------------------------------------------------------
void
CWidget::insertChild(CWidget * widget)
{
  pChildren_.push_back(widget);
}

//---------------------------------------------------------------------------
void
CWidget::removeChild(CWidget * widget)
{
  //pChildren_.remove(widget);
}

//---------------------------------------------------------------------------
CWidget *
CWidget::locate(int x, int y)
{
  CWidget * pWidget(0);

  // Only if we contain the location
  if(rectRelative_.contains(x, y) == true)
  {
    pWidget = this;

    // We have the location, is there a child placed on top of it?
    for(uint32_t iWidgetIdx(0); iWidgetIdx < pChildren_.size(); iWidgetIdx++)
    {
      CWidget * pChild = pChildren_[iWidgetIdx]->locate(x - left(), y - top());
      if(pChild != 0)
      {
        // Child placed on location
        pWidget = pChild;
        break;
      }
    }
  }

  return pWidget;
}

//---------------------------------------------------------------------------
void
CWidget::frame(bool frame)
{
  if(pWindowImpl_ != 0)
    pWindowImpl_->frame(frame);
}

//---------------------------------------------------------------------------
bool
CWidget::frame() const
{
  bool bRetVal(false);

  if(pWindowImpl_ != 0)
    bRetVal = pWindowImpl_->frame();

  return bRetVal;
}

//---------------------------------------------------------------------------
bool
CWidget::event(const CEvent & event)
{
  bool bHandled(true);

  switch(event.type())
  {
    case CEvent::etKeyboard:
      eventKey(static_cast<const CEventKey &>(event));
      break;
    case CEvent::etMouse:
      eventMouse(static_cast<const CEventMouse &>(event));
      break;
    case CEvent::etRedraw:
      if(bNeedRedraw_ == true)
      {
        bNeedRedraw_ = false;
        eventRedraw(static_cast<const CEventRedraw &>(event));
      }
      break;
    case CEvent::etError:
    default:
      ;
  };

  FORALLWIDGETS->event(event);

  return bHandled;
}

//---------------------------------------------------------------------------
bool
CWidget::eventKey(const CEventKey & event)
{
  return true;
}

//---------------------------------------------------------------------------
bool
CWidget::eventMouse(const CEventMouse & event)
{
  return true;
}

//---------------------------------------------------------------------------
bool
CWidget::eventRedraw(const CEventRedraw & event)
{
  return true;
}

//---------------------------------------------------------------------------
void
CWidget::redraw()
{
  bNeedRedraw_ = true;
}


};
