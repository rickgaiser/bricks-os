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


#include "widget.h"
#include "windowManager.h"


#define FORALLWIDGETS   for(uint32_t iWidgetIdx(0); iWidgetIdx < pChildren_.size(); iWidgetIdx++)(pChildren_[iWidgetIdx])


namespace twl
{


//---------------------------------------------------------------------------
CWidget::CWidget(CWidget * parent)
 : rectRelative_      (0, 0, 0, 0)
 , rectRelativeClient_(0, 0, 0, 0)
 , rectAbsolute_      (0, 0, 0, 0)
 , rectAbsoluteClient_(0, 0, 0, 0)
 , pParent_(parent)
 , bNeedRedraw_(true) // As soon as we're made visible, we need to redraw
 , bVisible_(false)   // Not visible by default
{
  if(pParent_ != 0)
  {
    pWindow_ = pParent_->pWindow_;
    pParent_->insertChild(this);
  }
  else
  {
    pWindow_ = bwm::windowManager.requestNewWindow();
    pWindow_->setEventHandler(this);
  }
}

//---------------------------------------------------------------------------
CWidget::~CWidget()
{
}

//---------------------------------------------------------------------------
void
CWidget::rect(const CRect & rect)
{
  rectRelative_       = rect;
  rectAbsolute_       = rect;

  if(pParent_ != 0)
  {
    rectAbsolute_.x(rectRelative_.x() + pParent_->rectAbsoluteClient().x());
    rectAbsolute_.y(rectRelative_.y() + pParent_->rectAbsoluteClient().y());
  }
  else
  {
    pWindow_->rect(rectAbsolute_);
  }

  rectRelativeClient_ = rectRelative_;
  rectAbsoluteClient_ = rectAbsolute_;

  // Send an event to ourselves
  this->event(CEventResize(rect.width(), rect.height()));
}

//---------------------------------------------------------------------------
void
CWidget::rect(int x, int y, int width, int height)
{
  this->rect(CRect(x, y, width, height));
}

//---------------------------------------------------------------------------
const CRect &
CWidget::rect()
{
  return rectRelative_;
}

//---------------------------------------------------------------------------
const CRect &
CWidget::rectClient()
{
  return rectRelativeClient_;
}

//---------------------------------------------------------------------------
const CRect &
CWidget::rectAbsolute()
{
  return rectAbsolute_;
}

//---------------------------------------------------------------------------
const CRect &
CWidget::rectAbsoluteClient()
{
  return rectAbsoluteClient_;
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
      CWidget * pChild = pChildren_[iWidgetIdx]->locate(x - this->rect().x(), y - this->rect().y());
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
CWidget::focus(bool focus)
{
  if(pWindow_ != 0)
    pWindow_->focus(focus);
}

//---------------------------------------------------------------------------
bool
CWidget::focus() const
{
  bool bRetVal(false);

  if(pWindow_ != 0)
    bRetVal = pWindow_->focus();

  return bRetVal;
}

//---------------------------------------------------------------------------
void
CWidget::frame(bool frame)
{
  if(pWindow_ != 0)
    pWindow_->frame(frame);
}

//---------------------------------------------------------------------------
bool
CWidget::frame() const
{
  bool bRetVal(false);

  if(pWindow_ != 0)
    bRetVal = pWindow_->frame();

  return bRetVal;
}

//---------------------------------------------------------------------------
void
CWidget::visible(bool visible)
{
  if(bVisible_ != visible)
  {
    bVisible_ = visible;
    if(bVisible_ == true)
      redraw();
    else if(pParent_ != 0)
      pParent_->redraw();
  }
}

//---------------------------------------------------------------------------
bool
CWidget::visible() const
{
  return bVisible_;
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
      if(bVisible_ == true)
      {
        if(bNeedRedraw_ == true)
        {
          bNeedRedraw_ = false;
          eventRedraw(static_cast<const CEventRedraw &>(event));
        }
        // Send event to all our child widgets as well
        FORALLWIDGETS->event(event);
      }
      break;
    case CEvent::etResize:
      eventResize(static_cast<const CEventResize &>(event));
      break;
    case CEvent::etError:
    default:
      ;
  };

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
bool
CWidget::eventResize(const CEventResize & event)
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
