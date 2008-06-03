#ifndef BWM_DESKTOP_H
#define BWM_DESKTOP_H


#include "frame.h"
#include "taskBar.h"


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
};


};


#endif
