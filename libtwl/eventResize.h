#ifndef TWL_EVENTRESIZE_H
#define TWL_EVENTRESIZE_H


#include "event.h"


namespace twl
{


class CEventResize
 : public CEvent
{
public:
  CEventResize(int width, int height);
  virtual ~CEventResize();

  int width() const;
  int height() const;

private:
  int iWidth_;
  int iHeight_;
};


};


#endif
