#include "desktop.h"


namespace twl
{


//---------------------------------------------------------------------------
CDesktop::CDesktop()
 : CFrame(0, CFrame::ftNone)
 , taskBar_(this, CFrame::ftRaised)
 , startButton_(&taskBar_, "Start")
 , window1_(0)
 , window2_(0)
{
  taskBar_.rect(0, height() - 15, width(), 15);
  taskBar_.visible(true);

  startButton_.rect(2, 2, 24, 10);
  startButton_.visible(true);

  window1_.rect(20, 20, 60, 40);
  window1_.visible(true);

  window2_.rect(100, 50, 80, 80);
  window2_.visible(true);
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
  window1_.rect(20, 20, 60, 40);
  window2_.rect(100, 50, 80, 80);

  return true;
}


};
