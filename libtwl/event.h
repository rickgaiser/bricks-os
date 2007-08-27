#ifndef TWL_EVENT_H
#define TWL_EVENT_H


namespace twl
{


class CEvent
{
public:
  enum EEventType
  {
      etError = 0
    , etKeyboard = 1
    , etMouse = 2
    , etRedraw = 3
  };

  CEvent(EEventType type = etError);
  virtual ~CEvent();

  EEventType type() const;

private:
  EEventType eType_;
};

class IWindowEventHandler
{
public:
  virtual ~IWindowEventHandler(){}

  virtual bool event(const CEvent & event) = 0;
};


};


#endif
