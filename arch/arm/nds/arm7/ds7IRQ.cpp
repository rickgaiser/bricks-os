#include "ds7IRQ.h"
#include "kernel/interruptManager.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"
#include "nds/interrupts.h"
#include "iostream"


// Assembler isr function calling our "C" isr function
extern "C" void __isr();

extern "C" void IntrMain();	// Prototype for assembly interrupt dispatcher

//---------------------------------------------------------------------------------
//void irqDummy(){}
//void irqDummy(){std::cout<<"INT!";}
//void irqDummy(){for(int i(0); i < 10; i++){}}
void irq00(){CInterruptManager::instance()->isr( 0, 0);}
void irq01(){CInterruptManager::instance()->isr( 1, 0);}
void irq02(){CInterruptManager::instance()->isr( 2, 0);}
void irq03(){CInterruptManager::instance()->isr( 3, 0);}
void irq04(){CInterruptManager::instance()->isr( 4, 0);}
void irq05(){CInterruptManager::instance()->isr( 5, 0);}
void irq06(){CInterruptManager::instance()->isr( 6, 0);}
void irq07(){CInterruptManager::instance()->isr( 7, 0);}
void irq08(){CInterruptManager::instance()->isr( 8, 0);}
void irq09(){CInterruptManager::instance()->isr( 9, 0);}
void irq10(){CInterruptManager::instance()->isr(10, 0);}
void irq11(){CInterruptManager::instance()->isr(11, 0);}
void irq12(){CInterruptManager::instance()->isr(12, 0);}
void irq13(){CInterruptManager::instance()->isr(13, 0);}
void irq14(){CInterruptManager::instance()->isr(14, 0);}
void irq15(){CInterruptManager::instance()->isr(15, 0);}
void irq16(){CInterruptManager::instance()->isr(16, 0);}
void irq17(){CInterruptManager::instance()->isr(17, 0);}
void irq18(){CInterruptManager::instance()->isr(18, 0);}
void irq19(){CInterruptManager::instance()->isr(19, 0);}
//---------------------------------------------------------------------------------

struct IntTable irqTable[MAX_INTERRUPTS];


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
isr()
{
  std::cout<<"INT!";

  // Find out who triggered the interrupt
  unsigned short iFlags(REG_IF & REG_IE);
  int iIntFlag(0x0001);
  for(int i(0); i < DS7_IRQ_COUNT; i++)
  {
    if((iFlags & iIntFlag) != 0)
    {
      // Handle interrupt
      CInterruptManager::instance()->isr(i, 0);

      break;
    }
    iIntFlag <<= 1;
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CDS7IRQ::CDS7IRQ()
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CDS7IRQ::~CDS7IRQ()
{
  for(int i(0); i < DS7_IRQ_COUNT; i++)
    CInterruptManager::instance()->detach(i, this);
}

// -----------------------------------------------------------------------------
int
CDS7IRQ::init()
{
//  IRQ_HANDLER = &::__isr;

  int i(0);
  irqTable[i].handler = irq00; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq01; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq02; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq03; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq04; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq05; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq06; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq07; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq08; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq09; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq10; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq11; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq12; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq13; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq14; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq15; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq16; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq17; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq18; irqTable[i].mask = (1<<i); i++;
  irqTable[i].handler = irq19; irqTable[i].mask = (1<<i); i++;
  
  IRQ_HANDLER = IntrMain;

  for(int i(0); i < DS7_IRQ_COUNT; i++)
    CInterruptManager::instance()->attach(i, this);

  return(0);
}

// -----------------------------------------------------------------------------
char *
CDS7IRQ::getDeviceName()
{
  return("DS7-IRQ");
}

// -----------------------------------------------------------------------------
void
CDS7IRQ::enable(unsigned int irq)
{
//  HAL::cli();
  // Set bit in IE(Interrupt Enable) register
  REG_IE |= (1 << irq);
//  HAL::sti();
}

// -----------------------------------------------------------------------------
void
CDS7IRQ::disable(unsigned int irq)
{
//  HAL::cli();
  // Clear bit in IE(Interrupt Enable) register
  REG_IE &= ~(1 << irq);
//  HAL::sti();
}

// -----------------------------------------------------------------------------
void
CDS7IRQ::ack(unsigned int irq)
{
  // ACK irq by writing irq bit to reg IF(Interrupt request Flags)
//  REG_IF |= (1 << irq);
}

// -----------------------------------------------------------------------------
void
CDS7IRQ::end(unsigned int irq)
{
}
