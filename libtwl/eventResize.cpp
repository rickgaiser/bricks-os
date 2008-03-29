#include "eventResize.h"


namespace twl
{


//---------------------------------------------------------------------------
CEventResize::CEventResize(int width, int height)
 : CEvent(CEvent::etResize)
 , iWidth_(width)
 , iHeight_(height)
{
}

//---------------------------------------------------------------------------
CEventResize::~CEventResize()
{
}

//---------------------------------------------------------------------------
int
CEventResize::width() const
{
  return iWidth_;
}

//---------------------------------------------------------------------------
int
CEventResize::height() const
{
  return iHeight_;
}


};
