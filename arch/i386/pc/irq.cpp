#include "kernel/interruptManager.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "hal.h"
#include "iostream"


#define PIC_MASTER_BASE 0x20  // Base IO addr
#define PIC_SLAVE_BASE  0xA0  // Base IO addr
#define IRQ_BASE        0x20
#define EOI_BYTE        0x20


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
  const char * msg[] =
  {
    // (0x00 - 0x1f) IA-32 Reserved Interrupts
    "Divide Error",
    "RESERVED",
    "NMI",
    "Breakpoint",              // INT3 instruction
    "Overflow",                // INTO instruction
    "BOUND Range Exceeded",    // BOUND instruction
    "Invalid Opcode",
    "Device Not Available",    // No coprocessor
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection",
    "Page Fault",
    "RESERVED",
    "x87 FPU Floating-Point Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    // (0x20 - 0x2f) IRQ interrupts
    "Timer (IRQ0)",
    "Keyboard (IRQ1)",
    "? (IRQ2)",
    "? (IRQ3)",
    "? (IRQ4)",
    "? (IRQ5)",
    "? (IRQ6)",
    "? (IRQ7)",
    "? (IRQ8)",
    "? (IRQ9)",
    "? (IRQ10)",
    "? (IRQ11)",
    "? (IRQ12)",
    "? (IRQ13)",
    "? (IRQ14)",
    "? (IRQ15)",
    // (0x30 - 0x30) Soft interrupts
    "Syscall"
  };
  
  if(regs->iIntNumber < 0x20)
  {
    // Handle CPU interrupts
    std::cout<<"CPU Interrupt("<<regs->iIntNumber<<"): "<<msg[regs->iIntNumber]<<std::endl;
    CCPU::halt();
  }
  else if(regs->iIntNumber < 0x30)
  {
    // Handle IRQs
    //std::cout<<"IRQ Interrupt("<<regs->iIntNumber<<"): "<<msg[regs->iIntNumber]<<std::endl;
    CInterruptManager::isr(regs->iIntNumber, regs);
  }
  else if(regs->iIntNumber < 0x31)
  {
    // Handle soft interrupts
    std::cout<<"Soft Interrupt("<<regs->iIntNumber<<"): "<<msg[regs->iIntNumber]<<std::endl;
    CCPU::halt();
  }
  else
  {
    // Handle unknown interrupts
    std::cout<<"Unknown Interrupt("<<regs->iIntNumber<<")"<<std::endl;
    CCPU::halt();
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CIRQ::CIRQ()
 : iMaskMaster_(0xff)
 , iMaskSlave_ (0xff)
{
  // Don't use constructor, use init function instead
}

// -----------------------------------------------------------------------------
CIRQ::~CIRQ()
{
  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::detach(i, this);
}

// -----------------------------------------------------------------------------
int
CIRQ::init()
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

  // Register IRQ interruts (0x20 - 0x2f)
  for(int i(0x20); i < 0x30; i++)
    CInterruptManager::attach(i, this);

  return 0;
}

// -----------------------------------------------------------------------------
char *
CIRQ::getDeviceName()
{
  return "i8259";
}

// -----------------------------------------------------------------------------
void
CIRQ::enable(unsigned int irq)
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
CIRQ::disable(unsigned int irq)
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
CIRQ::ack(unsigned int irq)
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
CIRQ::end(unsigned int irq)
{
}
