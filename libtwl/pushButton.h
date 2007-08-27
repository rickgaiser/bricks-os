#ifndef TWL_PUSHBUTTON_H
#define TWL_PUSHBUTTON_H


#include "frame.h"
#include "widget.h"
#include "event.h"
#include "eventKey.h"
#include "eventRedraw.h"


namespace twl
{


class CPushButton
 : public CFrame
{
public:
  CPushButton(CWidget * parent = 0, const char * string = 0);
  virtual ~CPushButton();

  virtual bool eventKey(const CEventKey & event);
  virtual bool eventRedraw(const CEventRedraw & event);

  virtual void pressed(); // Button pressed down
  virtual void released(); // Button released again
  virtual void clicked(); // Button pressed and then released

private:
  bool bPressed_;
  const char * string_;
};


};


#endif
