#ifndef GDT_H
#define GDT_H


#include "hal.h"


#define MAX_GLOBAL_DESCRIPTORS 0x20


class CGDT
{
public:
  CGDT();
  virtual ~CGDT();

  virtual int init();

  uint16_t newGD(uint32_t base,
                 uint32_t limit,  
                 uint8_t  access,
                 uint8_t  attribs);

private:
  SGlobalDescriptor    gd_[MAX_GLOBAL_DESCRIPTORS]; // Global Descriptor Table
  SDescriptorTableReg  gdtr_;                       // Global Descriptor Table Register
};


extern uint16_t selNull, selCodeSys, selDataSys;


#endif
