#include "pushButton.h"


namespace twl
{


//---------------------------------------------------------------------------
CPushButton::CPushButton(CWidget * parent, const char * string)
 : CFrame(parent, CFrame::ftRaised)
 , bPressed_(false)
 , string_(string)
{
}

//---------------------------------------------------------------------------
CPushButton::~CPushButton()
{
}

//---------------------------------------------------------------------------
bool
CPushButton::eventKey(const CEventKey & event)
{
  CWidget::eventKey(event);

  switch(event.key())
  {
    case kcA:
      if(event.pressed() == true)
      {
        setFrameType(CFrame::ftSunken);
        bPressed_ = true;
        redraw();
        pressed();
      }
      else
      {
        setFrameType(CFrame::ftRaised);
        bPressed_ = false;
        redraw();
        released();
        clicked();
      }
      break;
  };

  return true;
}

//---------------------------------------------------------------------------
bool
CPushButton::eventRedraw(const CEventRedraw & event)
{
  CFrame::eventRedraw(event);
/*
  if(string_ != 0)
  {
    if(bPressed_ == false)
      pWindow_->drawText(rectAbsolute().left() + 2, rectAbsolute().top() + 2, string_);
    else
      pWindow_->drawText(rectAbsolute().left() + 3, rectAbsolute().top() + 3, string_);
  }
*/
  return true;
}

//---------------------------------------------------------------------------
void
CPushButton::pressed()
{
}

//---------------------------------------------------------------------------
void
CPushButton::released()
{
}

//---------------------------------------------------------------------------
void
CPushButton::clicked()
{
  static CFrame * widget = 0;
  if(widget == 0)
  {
    widget = new CFrame(0, CFrame::ftSunken);
    widget->rect(60, 50, 120, 80);
  }
}


};
