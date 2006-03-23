#ifndef ARCHIRQ_H
#define ARCHIRQ_H


/**
 * \brief Interface for IRQ hardware
 */
class IArchIRQ
{
public:
  virtual char * getDeviceName() = 0;

  virtual void enable (unsigned int irq) = 0;
  virtual void disable(unsigned int irq) = 0;
  virtual void ack    (unsigned int irq) = 0;
  virtual void end    (unsigned int irq) = 0;
};


#endif
