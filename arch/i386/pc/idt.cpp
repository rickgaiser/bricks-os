#include "idt.h"
#include "gdt.h"
#include "hal.h"
#include "isrWrapper.h"


// -----------------------------------------------------------------------------
CIDT::CIDT()
{
}

// -----------------------------------------------------------------------------
CIDT::~CIDT()
{
}

// -----------------------------------------------------------------------------
int
CIDT::init()
{
  // Processor generated interrupts
  setID(0x00, selCodeSys, (unsigned long)&isr_0x00_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x01, selCodeSys, (unsigned long)&isr_0x01_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x02, selCodeSys, (unsigned long)&isr_0x02_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x03, selCodeSys, (unsigned long)&isr_0x03_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x04, selCodeSys, (unsigned long)&isr_0x04_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x05, selCodeSys, (unsigned long)&isr_0x05_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x06, selCodeSys, (unsigned long)&isr_0x06_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x07, selCodeSys, (unsigned long)&isr_0x07_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x08, selCodeSys, (unsigned long)&isr_0x08_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x09, selCodeSys, (unsigned long)&isr_0x09_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x0A, selCodeSys, (unsigned long)&isr_0x0A_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x0B, selCodeSys, (unsigned long)&isr_0x0B_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x0C, selCodeSys, (unsigned long)&isr_0x0C_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x0D, selCodeSys, (unsigned long)&isr_0x0D_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x0E, selCodeSys, (unsigned long)&isr_0x0E_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x0F, selCodeSys, (unsigned long)&isr_0x0F_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x10, selCodeSys, (unsigned long)&isr_0x10_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x11, selCodeSys, (unsigned long)&isr_0x11_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x12, selCodeSys, (unsigned long)&isr_0x12_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x13, selCodeSys, (unsigned long)&isr_0x13_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x14, selCodeSys, (unsigned long)&isr_0x14_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x15, selCodeSys, (unsigned long)&isr_0x15_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x16, selCodeSys, (unsigned long)&isr_0x16_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x17, selCodeSys, (unsigned long)&isr_0x17_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x18, selCodeSys, (unsigned long)&isr_0x18_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x19, selCodeSys, (unsigned long)&isr_0x19_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x1A, selCodeSys, (unsigned long)&isr_0x1A_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x1B, selCodeSys, (unsigned long)&isr_0x1B_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x1C, selCodeSys, (unsigned long)&isr_0x1C_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x1D, selCodeSys, (unsigned long)&isr_0x1D_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x1E, selCodeSys, (unsigned long)&isr_0x1E_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x1F, selCodeSys, (unsigned long)&isr_0x1F_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  // PIC generated interrupts
  setID(0x20, selCodeSys, (unsigned long)&isr_0x20_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x21, selCodeSys, (unsigned long)&isr_0x21_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x22, selCodeSys, (unsigned long)&isr_0x22_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x23, selCodeSys, (unsigned long)&isr_0x23_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x24, selCodeSys, (unsigned long)&isr_0x24_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x25, selCodeSys, (unsigned long)&isr_0x25_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x26, selCodeSys, (unsigned long)&isr_0x26_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x27, selCodeSys, (unsigned long)&isr_0x27_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x28, selCodeSys, (unsigned long)&isr_0x28_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x29, selCodeSys, (unsigned long)&isr_0x29_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x2A, selCodeSys, (unsigned long)&isr_0x2A_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x2B, selCodeSys, (unsigned long)&isr_0x2B_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x2C, selCodeSys, (unsigned long)&isr_0x2C_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x2D, selCodeSys, (unsigned long)&isr_0x2D_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x2E, selCodeSys, (unsigned long)&isr_0x2E_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  setID(0x2F, selCodeSys, (unsigned long)&isr_0x2F_wrapper, INT_GATE | SD_PRESENT | SD_DPL0 | SD_SYSTEM, 0);
  // Software generated interrupts
  setID(0x30, selCodeSys, (unsigned long)&isr_0x30_wrapper, INT_GATE | SD_PRESENT | SD_DPL3 | SD_SYSTEM, 0);

  // setting up the IDTR register
  idtr_.base  = (unsigned long)&id_[0];
  idtr_.limit = (sizeof(SInterruptDescriptor) * MAX_INTERRUPT_DESCRIPTORS) - 1;
  setIDTR(&idtr_);

  return 0;
}

// -----------------------------------------------------------------------------
void
CIDT::setID(uint8_t number, uint16_t selector, uint32_t offset, uint8_t access, uint8_t param_cnt)
{
  if(number < MAX_INTERRUPT_DESCRIPTORS)
  {
    id_[number].offset_l  = offset;
    id_[number].selector  = selector;
    id_[number].param_cnt = param_cnt;
    id_[number].access    = access;
    id_[number].offset_h  = offset >> 16;
  }
}
