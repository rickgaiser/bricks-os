#include "desktop.h"


namespace bwm
{


using namespace twl;


//---------------------------------------------------------------------------
CDesktop::CDesktop()
 : CWidget(0)
 , taskBar_(this)
 , window1_(0)
 , window2_(0)
{
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
  taskBar_.rect(0, height() - 13, width(), 13);
  taskBar_.visible(true);

  window1_.rect(20, 20, 60, 40);
  window1_.visible(true);

  window2_.rect(100, 50, 80, 80);
  window2_.visible(true);

  return true;
}


};
