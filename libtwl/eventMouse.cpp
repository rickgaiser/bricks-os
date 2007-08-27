#include "eventMouse.h"


namespace twl
{


//---------------------------------------------------------------------------
CEventMouse::CEventMouse(int xmove, int ymove)
 : CEvent(CEvent::etMouse)
 , iXMove_(xmove)
 , iYMove_(ymove)
{
}

//---------------------------------------------------------------------------
CEventMouse::~CEventMouse()
{
}

//---------------------------------------------------------------------------
int
CEventMouse::xmove() const
{
  return iXMove_;
}

//---------------------------------------------------------------------------
int
CEventMouse::ymove() const
{
  return iYMove_;
}


};
