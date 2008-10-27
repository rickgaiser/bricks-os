#include "kernel/debug.h"
#include "kernel/interruptManager.h"
#include "asm/arch/registers.h"
#include "asm/arch/memory.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "cache.h"


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


const char * sINTSource[] =
{
  "GS",
  "SBUS",
  "VBL_START",
  "VBL_END",
  "VIF0",
  "VIF1",
  "VU0",
  "VU1",
  "IPU",
  "TIMER0",
  "TIMER1"
};

IInterruptHandler * inthandlers[3];

uint32_t syscallTable[130] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*
  // 0
  0, 0, syscallSetCrtc, 0, syscallExit, 0, 0, 0, 0, 0,
  // 10
  0, 0, 0, 0, 0, 0, 0, 0, syscallAddDmacHandler, 0,
  // 20
  0, 0, syscallEnableDmac, 0, 0, 0, 0, 0, 0, 0,
  // 30
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 40
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 50
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 60
  syscallRFU60, syscallRFU61, 0, 0, syscallCreateSema, iDeleteSema, iSignalSema, iSignalSema, WaitSema, 0,
  // 70
  0, 0, 0, iDeleteSema, 0, 0, 0, 0, 0, 0,
  // 80
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 90
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 100
  syscallRFU100, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  // 110
  0, 0, 0, syscallSetGsIMR, 0, 0, 0, 0, 0, syscallSifSetDma,
  // 120
  syscallSifSetDChain, syscallSifSetReg, syscallSifGetReg, 0, 0, 0, 0, 0, 0, 0,
*/
};

// Entry point for V_COMMON exception handler
extern "C" void commonExceptionHandler();
// Entry point for V_INTERRUPT exception handler
extern "C" void interruptExceptionHandler();

// Function called by interruptExceptionHandler
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
  }
}

// -----------------------------------------------------------------------------
extern "C" void
errorHandler(uint32_t * regs)
{
  panic("Exception!\n");
}

// -----------------------------------------------------------------------------
extern "C" uint32_t
unknownSyscall(int nr)
{
  panic("Unknown syscall!\n");

  return 0;
}

// -----------------------------------------------------------------------------
extern "C" void
isr(pt_regs * regs)
{
  // Find out who triggered the interrupt
  uint32_t iCouse;
  iCouse = read_c0_cause();

  // Interrupt pending pin 0 (INTC)
  if(iCouse & INT_PENDING_0)
  {
    if(inthandlers[0] != NULL)
      inthandlers[0]->isr(0, regs);
  }

  // Interrupt pending pin 1 (DMAC)
  if(iCouse & INT_PENDING_1)
  {
    if(inthandlers[1] != NULL)
      inthandlers[1]->isr(1, regs);
  }

  // Interrupt pending pin 2 (not used)
  if(iCouse & INT_PENDING_2)
  {
    if(inthandlers[2] != NULL)
      inthandlers[2]->isr(2, regs);
  }
}

// -----------------------------------------------------------------------------
void
initExceptions()
{
  for(int i(0); i < 3; i++)
  {
    inthandlers[i] = NULL;
  }

  // SBUS
  REG_SBUS_SMFLG = (1 << 8) | (1 << 10);

//  install_exception_handler(V_TLB_REFILL, commonExceptionHandler);
//  install_exception_handler(V_COUNTER,    commonExceptionHandler);
//  install_exception_handler(V_DEBUG,      commonExceptionHandler);
//  install_exception_handler(V_COMMON,     commonExceptionHandler);
  install_exception_handler(V_INTERRUPT,  interruptExceptionHandler);
  flushDCacheAll();
  invalidateICacheAll();
}

// -----------------------------------------------------------------------------
void
setInterruptHandler(uint32_t nr, IInterruptHandler & handler)
{
  if(nr < 3)
  {
    inthandlers[nr] = &handler;
  }
  else
  {
    panic("setInterruptHandler: invalid nr: %d", nr);
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
  setInterruptHandler(MIPS_INT_0, *this);

  for(int i(0); i < MAX_INTERRUPTS; i++)
    CInterruptManager::attach(i, this);

  return 0;
}

// -----------------------------------------------------------------------------
void
CIRQ::isr(unsigned int irq, pt_regs * regs)
{
  uint64_t status = REG_INT_MASK;

  printk("interrupt from INTC(0x%x)\n", status);

  for(int i(0); i < MAX_INTERRUPTS; i++)
  {
    if(status & (1 << i))
    {
      printk("isr(%d)\n", i);
      // Interrupt found! Handle it!
      CInterruptManager::isr(i, 0);
    }
  }
}
