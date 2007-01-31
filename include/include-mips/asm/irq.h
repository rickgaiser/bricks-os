#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#define MAX_INTERRUPTS 0x30 // ???


// -----------------------------------------------------------------------------
class CIRQ
{
public:
  CIRQ();
  virtual ~CIRQ();

  int init();

  inline void enable (unsigned int irq){}
  inline void disable(unsigned int irq){}
  inline void ack    (unsigned int irq){}
  inline void end    (unsigned int irq){}
};


#endif
