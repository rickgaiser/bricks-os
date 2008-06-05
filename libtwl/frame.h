#ifndef TWL_FRAME_H
#define TWL_FRAME_H


#include "widget.h"
#include "eventRedraw.h"


namespace twl
{


class CFrame
 : public CWidget
{
public:
  enum EFrameType
  {
      ftNone
    , ftPlain
    , ftRaised
    , ftSunken
  };

  CFrame(CWidget * parent = 0, EFrameType type = ftNone);
  virtual ~CFrame();

  void setFrameType(EFrameType type);

  virtual bool eventRedraw(const CEventRedraw & event);

private:
  EFrameType eType_;
};


};


#endif
