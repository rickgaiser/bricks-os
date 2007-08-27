#include "eventKey.h"


namespace twl
{


//---------------------------------------------------------------------------
CEventKey::CEventKey(int key, bool pressed)
 : CEvent(CEvent::etKeyboard)
 , iKey_(key)
 , bPressed_(pressed)
{
}

//---------------------------------------------------------------------------
CEventKey::~CEventKey()
{
}

//---------------------------------------------------------------------------
int
CEventKey::key() const
{
  return iKey_;
}

//---------------------------------------------------------------------------
bool
CEventKey::pressed() const
{
  return bPressed_;
}


};
