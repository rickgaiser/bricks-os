#ifndef IDT_H
#define IDT_H


#include "hal.h"


#define MAX_INTERRUPT_DESCRIPTORS  0x31


class CIDT
{
public:
  CIDT();
  virtual ~CIDT();
  
  virtual int init();

  void setID(uint8_t  number,
             uint16_t selector,
             uint32_t offset,
             uint8_t  access,
             uint8_t  param_cnt);

private:
  SInterruptDescriptor   id_[MAX_INTERRUPT_DESCRIPTORS]; // Interrupt Descriptor Table
  SDescriptorTableReg    idtr_;                          // Interrupt Descriptor Table Register
};


#endif
