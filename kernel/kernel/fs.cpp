#include "kernel/fs.h"


// -----------------------------------------------------------------------------
CAFileIOBufferedRead::CAFileIOBufferedRead()
 : bufferRead_()
{
}

// -----------------------------------------------------------------------------
CAFileIOBufferedRead::~CAFileIOBufferedRead()
{
}

// -----------------------------------------------------------------------------
ssize_t
CAFileIOBufferedRead::read(void * buffer, size_t size, bool block)
{
  int iRetVal(0);
  uint8_t * data((uint8_t *)buffer);

  for(size_t i(0); i < size; i++)
  {
    if((i == 0) && (block == true))
      bufferRead_.get(data, true);
    else if(bufferRead_.get(data, false) == false)
      break;
    data++;
    iRetVal++;
  }

  return iRetVal;
}
