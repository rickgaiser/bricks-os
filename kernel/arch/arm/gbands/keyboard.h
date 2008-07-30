#ifndef ARCH_KEYBOARD_H
#define ARCH_KEYBOARD_H


#include "kernel/fs.h"
#include "kernel/interrupt.h"
#include "kernel/ringBuffer.h"
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
  virtual ssize_t read(void * buffer, size_t size, bool block = false);

private:
  CRingBuffer buffer_;
  uint16_t iKeys_;
  uint16_t iKeysXY_;
};


#endif
