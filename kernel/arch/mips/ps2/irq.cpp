#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"
#include "asm/arch/memory.h"
#include "asm/cpu.h"
#include "asm/irq.h"


// Exceptions
#define V_TLB_REFILL      0
#define V_COUNTER         1
#define V_DEBUG           2
#define V_COMMON          3
#define V_INTERRUPT       4

// Interrupts Pins (exception V_INTERRUPT)
#define INT_PENDING_0     (1<<10)
#define INT_PENDING_1     (1<<11)
#define INT_PENDING_2     (1<<15)

// Interrupts Controller (connected to INT_PENDING_0)
#define INT_GS            0
#define INT_SBUS          1
#define INT_VBLANK_START  2
#define INT_VBLANK_END    3
#define INT_VIF0          4
#define INT_VIF1          5
#define INT_VU0           6
#define INT_VU1           7
#define INT_IPU           8
#define INT_TIMER0        9
#define INT_TIMER1       10


// Assembler isr function calling our "C" isr function
extern "C" void interruptExceptionHandler();
// Function called by the above function
extern "C" void isr(pt_regs * regs) INTERRUPT_CODE;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
install_exception_handler(int type, void (*func)())
{
  vuint32_t * dest;

  if(type < 5)
  {
    dest = (uint32_t *)(KSEG0_START + type * 0x80);
    dest[0] = 0x00000000; // nop, required to fix CPU bug
    dest[1] = 0x00000000; // nop, required to fix CPU bug
    dest[2] = 0x08000000 | (0x03ffffff & (((uint32_t)func) >> 2)); // jump to function
    dest[3] = 0x00000000; // nop, required because of branch delay execution
//    flushDCacheAll();
//    invalidateICacheAll();
  }
}

// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
  // Find out who triggered the interrupt
  uint32_t iCouse;
  iCouse = read_c0_cause();

  panic("isr(0x%x)\n", iCouse);

  // Interrupt pending bit 0 (The one we need)
  if(iCouse & INT_PENDING_0)
  {
    uint64_t iFlags = REG_INT_MASK;

    for(int i(0); i < MAX_INTERRUPTS; i++)
    {
      if(iFlags & (1 << i))
      {
        // Interrupt found! Handle it!
        CInterruptManager::isr(i, 0);
      }
    }
  }

  // Interrupt pending bit 1
  if(iCouse & INT_PENDING_1)
  {
    panic("isr(0x%x)\n", iCouse);
  }

  // Interrupt pending bit 2
  if(iCouse & INT_PENDING_2)
  {
    panic("isr(0x%x)\n", iCouse);
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CIRQ::CIRQ()
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
  // SBUS
  REG_SBUS_SMFLG = (1 << 8) | (1 << 10);

  install_exception_handler(V_INTERRUPT, interruptExceptionHandler);

  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::attach(i, this);

  return 0;
}
