#ifndef I8042_MOUSE_H
#define I8042_MOUSE_H


#include "i8042.h"
#include "kernel/fs.h"
#include "inttypes.h"


#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_CENTER 0x04


struct SMouseData
{
  uint8_t keys;
  int8_t  x;
  int8_t  y;
} __attribute__ ((__packed__));


// -----------------------------------------------------------------------------
class CI8042Mouse
 : public CAFileIOBufferedRead
 , public I8042CallBack
{
public:
  CI8042Mouse(C8042 & driver);
  virtual ~CI8042Mouse();

  int init();

  // Inherited from ...
  virtual void i8042_callBack(uint8_t data);

private:
  C8042 & driver_;

  uint8_t iMouseByteNr_;

  union
  {
    SMouseData sMouseData_;
    uint8_t iMouseData_[3];
  };
};



#endif
