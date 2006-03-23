#include "kernel/interruptManager.h"
#include "i386IRQ.h"
#include "hal.h"


#define PIC_MASTER_BASE 0x20  // Base IO addr
#define PIC_SLAVE_BASE  0xA0  // Base IO addr
#define IRQ_BASE        0x20
#define EOI_BYTE        0x20


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
  CInterruptManager::instance()->isr(regs->iIntNumber, regs);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CI386IRQ::CI386IRQ()
 : iMaskMaster_(0xff)
 , iMaskSlave_ (0xff)
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CI386IRQ::~CI386IRQ()
{
  for(int i(0); i != IRQ_COUNT; i++)
    CInterruptManager::instance()->detach(i, this);
}

// -----------------------------------------------------------------------------
int
CI386IRQ::init()
{
  // Initialize master PIC
  outb(0x11, PIC_MASTER_BASE);             // ICW1, ICW4 needed, Cascade mode, Call address interval of 8 bytes, Edge triggered mode
  outb(IRQ_BASE, PIC_MASTER_BASE + 1);     // ICW2, Interrupt base address
  outb(0x04, PIC_MASTER_BASE + 1);         // ICW3, Slave PIC connected to IRQ2 of master
  outb(0x01, PIC_MASTER_BASE + 1);         // ICW4, 8086/8088 mode, Software handles EOI
  outb(iMaskMaster_, PIC_MASTER_BASE + 1); // Disable all interrupts

  // Initialize slave PIC
  outb(0x11, PIC_SLAVE_BASE);              // ICW1, ICW4 needed, Cascade mode, Call address interval of 8 bytes, Edge triggered mode
  outb(IRQ_BASE + 8, PIC_SLAVE_BASE + 1);  // ICW2, Interrupt base address
  outb(0x02, PIC_SLAVE_BASE + 1);          // ICW3, Slave PIC connected to IRQ2 of master
  outb(0x01, PIC_SLAVE_BASE + 1);          // ICW4, 8086/8088 mode, Software handles EOI
  outb(iMaskSlave_, PIC_SLAVE_BASE + 1);   // Disable all interrupts

  // Register objects (Processor Generated Interrupts)
  for(int i(0); i < 0x20; i++)
    CInterruptManager::instance()->attach(i, this);
  // Register objects (IRQ 0x20 - 0x2f)
  for(int i(0); i < IRQ_COUNT; i++)
    CInterruptManager::instance()->attach(IRQ_BASE + i, this);

  return(0);
}

// -----------------------------------------------------------------------------
char *
CI386IRQ::getDeviceName()
{
  return("i8259");
}

// -----------------------------------------------------------------------------
void
CI386IRQ::enable(unsigned int irq)
{
  irq -= IRQ_BASE;

  if(irq < 8)
  {
    iMaskMaster_ &= ~(1<<irq);
    outb(iMaskMaster_, PIC_MASTER_BASE + 1);
  }
  else if(irq < 16)
  {
    irq -= 8;
    iMaskSlave_ &= ~(1<<irq);
    outb(iMaskSlave_, PIC_SLAVE_BASE + 1);
  }
}

// -----------------------------------------------------------------------------
void
CI386IRQ::disable(unsigned int irq)
{
  irq -= IRQ_BASE;

  if(irq < 8)
  {
    iMaskMaster_ |= (1<<irq);
    outb(iMaskMaster_, PIC_MASTER_BASE + 1);
  }
  else if(irq < 16)
  {
    irq -= 8;
    iMaskSlave_ |= (1<<irq);
    outb(iMaskSlave_, PIC_SLAVE_BASE + 1);
  }
}

// -----------------------------------------------------------------------------
void
CI386IRQ::ack(unsigned int irq)
{
  irq -= IRQ_BASE;

  if(irq < 16)
  {
    outb(EOI_BYTE, PIC_MASTER_BASE);
    if(irq >= 8)
      outb(EOI_BYTE, PIC_SLAVE_BASE);
  }
}

// -----------------------------------------------------------------------------
void
CI386IRQ::end(unsigned int irq)
{
}
