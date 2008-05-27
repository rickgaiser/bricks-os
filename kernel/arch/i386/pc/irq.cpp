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
uint8_t
v86_fetch8(pt_regs * regs)
{
  uint8_t data;

  data = *(uint8_t *)from_v86_addr(regs->cs, regs->eip);
  regs->eip++;

  return data;
}

// -----------------------------------------------------------------------------
void
v86_push16(pt_regs * regs, uint16_t data)
{
  regs->user_esp = (regs->user_esp - 2) & 0xffff;
  *(uint16_t *)from_v86_addr(regs->user_ss, regs->user_esp) = data;
}

// -----------------------------------------------------------------------------
uint16_t
v86_pop16(pt_regs * regs)
{
  uint16_t data;

  data = *(uint16_t *)from_v86_addr(regs->user_ss, regs->user_esp);
  regs->user_esp = (regs->user_esp + 2) & 0xffff;

  return data;
}

// -----------------------------------------------------------------------------
void
v86_push32(pt_regs * regs, uint32_t data)
{
  regs->user_esp = (regs->user_esp - 4) & 0xffff;
  *(uint32_t *)from_v86_addr(regs->user_ss, regs->user_esp) = data;
}

// -----------------------------------------------------------------------------
uint32_t
v86_pop32(pt_regs * regs)
{
  uint32_t data;

  data = *(uint32_t *)from_v86_addr(regs->user_ss, regs->user_esp);
  regs->user_esp = (regs->user_esp + 4) & 0xffff;

  return data;
}

// -----------------------------------------------------------------------------
#define PFX_ES    0x001
#define PFX_CS    0x002
#define PFX_SS    0x004
#define PFX_DS    0x008
#define PFX_FS    0x010
#define PFX_GS    0x020
#define PFX_OP32  0x040
#define PFX_ADR32 0x080
#define PFX_LOCK  0x100
#define PFX_REPNE 0x200
#define PFX_REP   0x400
// -----------------------------------------------------------------------------
int
v86gpf_handler(pt_regs * regs)
{
  int      iRetVal(0);
  uint32_t prefix(0);
  bool     bPrefixLoop(true);
  uint8_t  data;

  //printk("v86 gpf\n");

  while(bPrefixLoop)
  {
    data = v86_fetch8(regs);
    switch(data)
    {
      case 0x26:
        prefix |= PFX_ES;
        break;
      case 0x2e:
        prefix |= PFX_CS;
        break;
      case 0x36:
        prefix |= PFX_SS;
        break;
      case 0x3e:
        prefix |= PFX_DS;
        break;
      case 0x64:
        prefix |= PFX_FS;
        break;
      case 0x65:
        prefix |= PFX_GS;
        break;
      case 0x66:
        prefix |= PFX_OP32;
        break;
      case 0x67:
        prefix |= PFX_ADR32;
        break;
      case 0xf0:
        prefix |= PFX_LOCK;
        break;
      case 0xf2:
        prefix |= PFX_REPNE;
        break;
      case 0xf3:
        prefix |= PFX_REP;
        break;
      default:
        bPrefixLoop = false;
    };
  }

  switch(data)
  {
    case 0xcd: // int xx
    {
      uint16_t * base = 0;
      data = v86_fetch8(regs);
      printk("v86 gpf: int 0x%x\n", data);
      // push eip, cs, eflags
      v86_push16(regs, regs->eflags);
      v86_push16(regs, regs->cs);
      v86_push16(regs, regs->eip);
      // int xx
      regs->cs  = base[data * 2 + 1];
      regs->eip = base[data * 2 + 0];
      break;
    }
    case 0xcf: // iret
    {
      printk("v86 gpf: iret\n");
      // pop eip, cs, eflags
      if(prefix & PFX_OP32)
      {
        if(regs->user_esp > 0x0ff4)
          iRetVal = 1; // Exit
        regs->eip    = v86_pop32(regs);
        regs->cs     = v86_pop32(regs);
        regs->eflags = v86_pop32(regs);
      }
      else
      {
        if(regs->user_esp > 0x0ffa)
          iRetVal = 1; // Exit
        regs->eip    = v86_pop16(regs);
        regs->cs     = v86_pop16(regs);
        regs->eflags = (regs->eflags & 0xffff0000) | v86_pop16(regs);
      }
      break;
    }
    case 0x9c: // pushf
      printk("v86 gpf: pushf\n");
      if(prefix & PFX_OP32)
        v86_push32(regs, regs->eflags);
      else
        v86_push16(regs, regs->eflags);
      break;
    case 0x9d: // popf
      printk("v86 gpf: popf\n");
      if(prefix & PFX_OP32)
      {
        if(regs->user_esp > 0x0ffc)
          iRetVal = -1; // Exit
        regs->eflags = v86_pop32(regs);
      }
      else
      {
        if(regs->user_esp > 0x0ffe)
          iRetVal = -1; // Exit
        regs->eflags = (regs->eflags & 0xffff0000) | v86_pop16(regs);
      }
      break;
    case 0xfa: // cli
      printk("v86 gpf: cli\n");
      regs->eflags &= ~I386_IE_FLAG;
      break;
    case 0xfb: // sti
      printk("v86 gpf: sti\n");
      regs->eflags |= I386_IE_FLAG;
      break;
    case 0xe4: // in al,imm8
      panic("v86 gpf: in al,imm8\n");
      break;
    case 0xe6: // out imm8,al
      panic("v86 gpf: out imm8,al\n");
      break;
    case 0xe5: // in eax,imm8
      panic("v86 gpf: in eax,imm8\n");
      break;
    case 0xe7: // out imm8,eax
      panic("v86 gpf: out imm8,eax\n");
      break;
    case 0x6c: // insb
      panic("v86 gpf: insb\n");
      break;
    case 0x6e: // outsb
      panic("v86 gpf: outsb\n");
      break;
    case 0xec: // in al,dx
      panic("v86 gpf: in al,dx\n");
      break;
    case 0xee: // out dx,al
      panic("v86 gpf: out dx,al\n");
      break;
    case 0x6d: // insw/insd
      panic("v86 gpf: insw/insd\n");
      break;
    case 0x6f: // outsw/outsd
      panic("v86 gpf: outsw/outsd\n");
      break;
    case 0xed: // in eax,dx
      panic("v86 gpf: in eax,dx\n");
      break;
    case 0xef: // out dx,eax
      panic("v86 gpf: out dx,eax\n");
      break;
    default:
      panic("v86 gpf, addr: 0x%x, inst: 0x%x\n", from_v86_addr(regs->cs, regs->eip), data);
  };

  return iRetVal;
}

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
      panic("CPU Interrupt(0x%x): %s, addr: 0x%x\n", regs->iIntNumber, msg[regs->iIntNumber], regs->eip);
      break;
    case 0x0d:
      if(regs->eflags & (1<<17))
      {
        if(v86gpf_handler(regs) != 0)
        {
          printk("v86 gpf: returning to caller\n");
          // Load return stack or jump to TSS
          CTaskManager::pCurrentThread_->runJump();
        }
      }
      else
        panic("GPF, addr: 0x%x\n", regs->eip);
      break;

    // Handle IRQs
    case 0x20:  // Timer
      // Task/Thread stuff
      CTaskManager::updateSleepers();
      CTaskManager::removeDestroyed();
      if(CTaskManager::schedule() == true)
      {
        // Ack interrupt (normally interrupt manager will do this)
        outb(EOI_BYTE, PIC_MASTER_BASE);
        // Load return stack or jump to TSS
        CTaskManager::pCurrentThread_->runJump();
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
      //printk("IRQ Interrupt(0x%x): %s\n", regs->iIntNumber, msg[regs->iIntNumber]);
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
            CTaskManager::pCurrentThread_->runJump();
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
