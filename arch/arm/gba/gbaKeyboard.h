#ifndef GBAKEYBOARD_H
#define GBAKEYBOARD_H


#include "kernel/file.h"
#include "kernel/interrupt.h"


class CGBAKeyboard
 : public IFileIO
 , public IInterruptServiceRoutine
{
public:
  CGBAKeyboard();
  virtual ~CGBAKeyboard();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  // Inherited from IDriver
  virtual int read(void * data, size_t size);

private:
  volatile int iBufferCount_;
  int iKey_;
};


#endif
