#ifndef TWL_EVENTMOUSE_H
#define TWL_EVENTMOUSE_H


#include "event.h"


namespace twl
{


class CEventMouse
 : public CEvent
{
public:
  CEventMouse(int xmove, int ymove);
  virtual ~CEventMouse();

  int xmove() const;
  int ymove() const;

private:
  int iXMove_;
  int iYMove_;
};


};


#endif
