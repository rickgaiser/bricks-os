#ifndef IRQ_H
#define IRQ_H


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
