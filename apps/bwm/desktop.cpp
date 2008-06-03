#include "desktop.h"


namespace bwm
{


using namespace twl;


//---------------------------------------------------------------------------
CDesktop::CDesktop()
 : CWidget(0)
 , taskBar_(this)
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
  if(rect().height() < 480)
    taskBar_.rect(0, rect().height() - 14, rect().width(), 14); // Tiny mode
  else
    taskBar_.rect(0, rect().height() - 28, rect().width(), 28); // Normal mode

  taskBar_.visible(true);

  return true;
}


};
