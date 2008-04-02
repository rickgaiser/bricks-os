#ifndef BWM_TASKBAR_H
#define BWM_TASKBAR_H


#include "widget.h"
#include "pushButton.h"


namespace bwm
{


class CTaskBar
 : public twl::CWidget
{
public:
  CTaskBar(twl::CWidget * parent = 0);
  virtual ~CTaskBar();

  virtual bool eventRedraw(const twl::CEventRedraw & event);
  virtual bool eventResize(const twl::CEventResize & event);

private:
  twl::CPushButton startButton_;
};


};


#endif
