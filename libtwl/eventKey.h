#ifndef TWL_EVENTKEY_H
#define TWL_EVENTKEY_H


#include "event.h"


namespace twl
{


enum EKeyCodes
{
    kcUP
  , kcDOWN
  , kcLEFT
  , kcL1         // Game Console Key
  , kcL2         // Game Console Key
  , kcRIGHT
  , kcR1         // Game Console Key
  , kcR2         // Game Console Key
  , kcSTART      // Game Console Key
  , kcSELECT     // Game Console Key
  , kcTRIANGLE   // Game Console Key (Sony)
  , kcCROSS      // Game Console Key (Sony)
  , kcSQUARE     // Game Console Key (Sony)
  , kcCIRCEL     // Game Console Key (Sony)
  , kcA
  , kcB
  , kcX
  , kcY
};

class CEventKey
 : public CEvent
{
public:
  CEventKey(int key, bool bPressed);
  virtual ~CEventKey();

  int key() const;
  bool pressed() const;

private:
  int iKey_;
  bool bPressed_;
};


};


#endif
