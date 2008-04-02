#ifndef TWL_DESKTOP_H
#define TWL_DESKTOP_H


#include "frame.h"
#include "taskBar.h"
#include "glDemo1.h"
#include "glDemo2.h"


namespace bwm
{


class CDesktop
 : public twl::CWidget
{
public:
  CDesktop();
  virtual ~CDesktop();

  virtual bool eventRedraw(const twl::CEventRedraw & event);
  virtual bool eventResize(const twl::CEventResize & event);

private:
  CTaskBar taskBar_;
  CGLDemo1 window1_;
  CGLDemo2 window2_;
};


};


#endif
