#include "irq.h"
#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "kernel/srr_k.h"
#include "kernel/task.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "asm/hal.h"


#define PIC_MASTER_BASE 0x20  // Base IO addr
#define PIC_SLAVE_BASE  0xA0  // Base IO addr
#define IRQ_BASE        0x20
#define EOI_BYTE        0x20


extern CIRQ cIRQ;


// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
  bool bTimeout(false);

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
    // (0x20 - 0x2f, IRQ0 - IRQ15) Intel 8259 IRQ interrupts
    "Timer",                   // Intel 8253/8254
    "Keyboard",                // Intel 8042
    "Cascade",
    "COM 2/4",                 // 8250
    "COM 1/3",                 // 8250
    "LPT2",                    // Intel 8255
    "Floppy",                  // Intel 82072A
    "LPT1",                    // Intel 8255
    "RTC",
    "?",
    "?",
    "?",
    "Mouse",                   // Intel 8042
    "Math Coprocessor",
    "Pimary IDE",
    "Secondary IDE",
    // (0x30 - 0x30) Soft interrupts
    "Syscall"
  };

  switch(regs->iIntNumber)
  {
    // Handle CPU interrupts
    case 0x07: // Device Not Available
    {
      // FIXME: Save floating point registers
      // ...

      // Clear the TS (task switch) flag if present
      uint32_t cr0 = getCR0();
      if(cr0 & CR0_TS)
        setCR0(cr0 & (~CR0_TS));
      else
        panic("Device Not Available\n");

      break;
    }
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d: // GPF, handled by TSS task
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
      panic("CPU Interrupt(0x%x): %s, addr: 0x%x, error: 0x%x\n", regs->iIntNumber, msg[regs->iIntNumber], regs->eip, regs->iErrorCode);
      break;

    // Handle IRQs
    case 0x20:  // Timer
      // Timer interrupt should be handled by a TSS task
      panic("Timer interrupt received!\n");
      break;
    case 0x21: // Keyboard
    case 0x22: // Cascade
    case 0x23: // COM 2/4
    case 0x24: // COM 1/3
    case 0x25: // LPT2
    case 0x26: // Floppy
    case 0x27: // LPT1
    case 0x28: // RTC
    case 0x29: // Free
    case 0x2a: // Free
    case 0x2b: // Free
    case 0x2c: // Mouse
    case 0x2d: // Math Coprocessor
    case 0x2e: // Pimary IDE
    case 0x2f: // Secondary IDE
      //printk("IRQ Interrupt(0x%x): %s\n", regs->iIntNumber, msg[regs->iIntNumber]);
      cIRQ.ack(regs->iIntNumber);
      CInterruptManager::isr(regs->iIntNumber, regs);
      break;

    // Bricks System Calls
    case 0x30:
    {
      switch(regs->eax)
      {
        case 1:
          regs->eax = k_msgSend((int)regs->ebx, (const void *)regs->ecx, (int)regs->edx, (void *)regs->esi, (int)regs->edi);
          break;
        case 2:
          regs->eax = k_msgReceive((int)regs->ebx, (void *)regs->ecx, (int)regs->edx);
          break;
        case 3:
          regs->eax = k_msgReply((int)regs->ebx, (int)regs->ecx, (const void *)regs->edx, (int)regs->esi);
          break;
        default:
          panic("ERROR: Unknown int 0x30 System Call(eax=0x%x)\n", regs->eax);
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
          CTaskManager::pCurrentThread_->state(TS_DESTROY);
          if(CTaskManager::schedule() == true)
            CTaskManager::pCurrentThread_->impl().runJump();
          break;
        case 4:
          // write
          printk((char *)regs->ecx);
          break;
        default:
          panic("ERROR: Unknown int 0x80 System Call(eax=0x%x)\n", regs->eax);
      };
      break;
    }

    // Handle unknown interrupts
    default:
      panic("Unknown Interrupt(0x%x)\n", regs->iIntNumber);
  };

  // Run scheduler
  if(CTaskManager::schedule(true) == true)
  {
    // Load return stack
    //CTaskManager::pCurrentThread_->impl().runReturn();
    // Jump to task
    CTaskManager::pCurrentThread_->impl().runJump();
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

  // Enable interrupts from slave PIC
  this->enable(0x22);

  return 0;
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
