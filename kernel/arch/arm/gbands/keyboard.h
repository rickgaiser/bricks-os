#ifndef ARCH_KEYBOARD_H
#define ARCH_KEYBOARD_H


#include "kernel/fs.h"
#include "kernel/interrupt.h"
#include "asm/arch/memory.h"


class CGBAKeyboard
 : public IInterruptServiceRoutine
 , public CAFileIOBufferedRead
{
public:
  CGBAKeyboard();
  virtual ~CGBAKeyboard();

  virtual int init();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

private:
  uint16_t iKeys_;
#ifdef NDS7
  uint16_t iKeysXY_;
#endif
};


#endif
