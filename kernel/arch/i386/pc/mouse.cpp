#include "mouse.h"
#include "kernel/debug.h"


// -----------------------------------------------------------------------------
CI8042Mouse::CI8042Mouse(C8042 & driver)
 : driver_(driver)
 , iMouseByteNr_(0)
{
}

// -----------------------------------------------------------------------------
CI8042Mouse::~CI8042Mouse()
{
}

// -----------------------------------------------------------------------------
int
CI8042Mouse::init()
{
  uint8_t dummy;

  // Register our callback handler
  driver_.registerHandler(1, this);

  //Tell the mouse to use default settings
  driver_.writeData(1, 0xf6);
  driver_.readData(1, &dummy);

  //Enable the mouse
  driver_.writeData(1, 0xf4);
  driver_.readData(1, &dummy);

  return 0;
}

// -----------------------------------------------------------------------------
void
CI8042Mouse::i8042_callBack(uint8_t data)
{
  iMouseData_[iMouseByteNr_] = data;
  iMouseByteNr_++;

  if(iMouseByteNr_ == 3)
  {
    iMouseByteNr_ = 0;
    //printk("Mouse keys=0x%x, x=%d, y=%d\n", sMouseData_.keys, sMouseData_.x, sMouseData_.y);
  }
}

// -----------------------------------------------------------------------------
int
CI8042Mouse::read(void * data, size_t size)
{
  return 0;
}
