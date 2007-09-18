#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "kernel/srr_k.h"
#include "kernel/task.h"
#include "asm/cpu.h"
#include "asm/irq.h"
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
  //printk("isr 0x%x\n", regs->iIntNumber);

  static const char * msg[] =
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

  switch(regs->iIntNumber)
  {
    // Handle CPU interrupts
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x0f:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
      panic("CPU Interrupt(%d): %s, addr: %d\n", regs->iIntNumber, msg[regs->iIntNumber], regs->eip);
      break;

    // Handle IRQs
    case 0x20:  // Timer
      // Run scheduler
      if(CTaskManager::schedule() == true)
      {
        // Ack interrupt (normally interrupt manager will do this)
        outb(EOI_BYTE, PIC_MASTER_BASE);
        // Load return stack or jump to TSS
        CTaskManager::pCurrentTask_->run();
      }
      else
      {
        // Ack interrupt (normally interrupt manager will do this)
        outb(EOI_BYTE, PIC_MASTER_BASE);
      }
      break;
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
      //printk("IRQ Interrupt(%d): %s\n", regs->iIntNumber, msg[regs->iIntNumber]);
      CInterruptManager::isr(regs->iIntNumber, regs);
      break;

    // Bricks System Calls
    case 0x30:
    {
      switch(regs->eax)
      {
        case 1:
          printk("msgSend\n");
          regs->eax = k_msgSend((int)regs->ebx, (const void *)regs->ecx, (int)regs->edx, (void *)regs->edi, (int)regs->esi);
          break;
        case 2:
          printk("msgReceive\n");
          regs->eax = k_msgReceive((int)regs->ebx, (void *)regs->ecx, (int)regs->edx);
          break;
        case 3:
          printk("msgReply\n");
          regs->eax = k_msgReply((int)regs->ebx, (int)regs->ecx, (const void *)regs->edx, (int)regs->edi);
          break;
        default:
          printk("ERROR: Unknown int 0x30 System Call(%d)\n", regs->eax);
      };
      break;
    }
    // Linux System Calls
    case 0x80:
    {
      switch(regs->eax)
      {
        case 1:
          // exit
          CTaskManager::removeTask(CTaskManager::pCurrentTask_);
          CTaskManager::pCurrentTask_->run();
          break;
        case 4:
          // write
          printk((char *)regs->ecx);;
          break;
        default:
          printk("ERROR: Unknown int 0x80 System Call(%d)\n", regs->eax);
      };
      break;
    }

    // Handle unknown interrupts
    default:
      panic("Unknown Interrupt(%d)\n", regs->iIntNumber);
  };
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
