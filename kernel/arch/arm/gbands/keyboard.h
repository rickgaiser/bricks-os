#ifndef ARCH_KEYBOARD_H
#define ARCH_KEYBOARD_H


#include "kernel/fs.h"
#include "kernel/interrupt.h"
#include "asm/arch/memory.h"


class CGBAKeyboard
 : public IFileIO
 , public IInterruptServiceRoutine
{
public:
  CGBAKeyboard();
  virtual ~CGBAKeyboard();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

  // Inherited from IFileIO
  virtual ssize_t read(void * buffer, size_t size, loff_t * = 0);

private:
  volatile int iBufferCount_;
  int iKey_;
};


#endif
