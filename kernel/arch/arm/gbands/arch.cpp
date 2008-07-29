#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "kernel/task.h"

#include "asm/arch/config.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "asm/task.h"

#include "drivers.h"
#include "timer.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING


#ifdef GBA
extern char __iheap_start, __iwram_top;
extern char __eheap_start, __eheap_end;
#define HEAP_START ((uint32_t)(&__eheap_start))
#define HEAP_END   ((uint32_t)(&__eheap_end))
#elif NDS7
extern char _end, __iwram_top;
#define HEAP_START ((uint32_t)(&_end))
#define HEAP_END   ((uint32_t)(&__iwram_top))
#elif NDS9
extern char _end, __eheap_end;
#define HEAP_START ((uint32_t)(&_end))
#define HEAP_END   ((uint32_t)(&__eheap_end))
#endif


CIRQ           cIRQ;

#ifdef CONFIG_DEBUGGING
#if defined(GBA) || defined(NDS9)
CGBADebugScreen cDebug;
#ifdef NDS9
CGBADebugScreen cDebugARM7;
#endif // NDS9
#endif // defined(GBA) || defined(NDS9)
#endif // CONFIG_DEBUGGING


extern "C"
{
  void __gxx_personality_v0()
  {
  }
  void __aeabi_atexit()
  {
  }
  void __cxa_pure_virtual()
  {
  }
#ifdef NDS
  void initSystem(void)
  {
  }
#endif // NDS
}

// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

  if(cIRQ.init() == -1)
    iRetVal = -1;

#ifdef CONFIG_DEBUGGING
#if defined(GBA) || defined(NDS9)
  cDebug.init();
  pDebug = &cDebug;
#ifdef NDS9
  cDebugARM7.init(BOTTOM_SCREEN);
#endif // NDS9
#endif // defined(GBA) || defined(NDS9)
#endif // CONFIG_DEBUGGING

  task_init();
  setTimerFrequency(2, 100.0f);
  cIRQ.enable(5);

  // Let CPU flag control interrupt state
  local_irq_disable();  // Disable in cpu interrupt enable flag
  REG_IME = 1;          // Enable REG_IME interrupt enable flag

  printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  // Enable interrupts
  local_irq_enable();
  printk("Interrupts...OK\n");

  // Initialize drivers
  init_drivers();

#ifdef GBA
  printk("GBA arch ready\n");
#elif NDS7
  printk("NDS-ARM7 arch ready\n");
#elif NDS9
  printk("NDS-ARM9 arch ready\n");
#endif

  return bricks_main();
}
