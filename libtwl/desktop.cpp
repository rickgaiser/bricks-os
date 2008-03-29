#include "desktop.h"


namespace twl
{


//---------------------------------------------------------------------------
CDesktop::CDesktop()
 : CFrame(0, CFrame::ftNone)
 , taskBar_(this, CFrame::ftRaised)
 , startButton_(&taskBar_, "Start")
{
  taskBar_.rect(0, height() - 15, width(), 15);
  taskBar_.visible(true);

  startButton_.rect(2, 2, 24, 10);
  startButton_.visible(true);
}

//---------------------------------------------------------------------------
CDesktop::~CDesktop()
{
}

//---------------------------------------------------------------------------
bool
CDesktop::eventRedraw(const CEventRedraw & event)
{
  pWindow_->fill(clDesktop);
  pWindow_->drawText(0, 0, "Bricks-OS", clWhite);

  return true;
}

//---------------------------------------------------------------------------
bool
CDesktop::eventResize(const CEventResize & event)
{
  taskBar_.rect(0, height() - 15, width(), 15);
  startButton_.rect(2, 2, 24, 10);

  return true;
}


};
