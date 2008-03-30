#ifndef TWL_DESKTOP_H
#define TWL_DESKTOP_H


#include "frame.h"
#include "eventRedraw.h"
#include "pushButton.h"
#include "glDemo1.h"


namespace twl
{


class CDesktop
 : public CFrame
{
public:
  CDesktop();
  virtual ~CDesktop();

  virtual bool eventRedraw(const CEventRedraw & event);
  virtual bool eventResize(const CEventResize & event);

private:
  CFrame taskBar_;
  CPushButton startButton_;
  CGLDemo1 window_;
};


};


#endif
