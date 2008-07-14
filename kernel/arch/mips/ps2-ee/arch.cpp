#include "kernel/bricks.h"
#include "kernel/debug.h"
#include "kernel/memoryManager.h"
#include "kernel/task.h"

#include "asm/arch/config.h"
#include "asm/cpu.h"
#include "asm/irq.h"
#include "asm/task.h"

#include "dma.h"
#include "sif.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


#ifdef CONFIG_BUILTIN_MM
extern char _end;
extern char _heap_size;
#define HEAP_START ((uint32_t)(&_end))
#define HEAP_END   ((uint32_t)(&_end + 4*1024*1024)) // FIXME: (&_end + &_heap_size))
#endif // CONFIG_BUILTIN_MM


//CIRQ           cIRQ;
CIOP           cIOP;

#ifdef CONFIG_DEBUGGING
CPS2DebugScreen cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
CPS2VideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


// -----------------------------------------------------------------------------
int
main(int, char *[])
{
#ifdef CONFIG_BUILTIN_MM
  init_heap((void *)HEAP_START, HEAP_END - HEAP_START);
#endif // CONFIG_BUILTIN_MM

//  cIRQ.init();

  // Initialize the DMA controller
  // Needed for DMA transfer to GS, used by cDebug
  dmaInitialize();

#ifdef CONFIG_DEBUGGING
  cDebug.init();
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CPS2VideoDevice;
#endif // CONFIG_FRAMEBUFFER

  task_init();

  // Initialize IOP
  cIOP.init();
  // Wait for IOP
  printk("Waiting for IOP...");
  while(cIOP.sync());
  printk("ready\n");

#ifdef CONFIG_BUILTIN_MM
  printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);
#endif // CONFIG_BUILTIN_MM

  printk("PS2 arch ready\n");

  return bricks_main();
}
