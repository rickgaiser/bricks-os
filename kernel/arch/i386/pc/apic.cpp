#include "apic.h"
#include "cpuid.h"
#include "asm/arch/config.h"
#include "asm/hal.h"
#include "asm/task.h"
#include "kernel/task.h"
#include "kernel/debug.h"


uint32_t * pApicID = (uint32_t *)(0xfee00020);
uint32_t * pApicVersion = (uint32_t *)(0xfee00030);

void
init_apic()
{
  if(CPU::hasAPIC())
  {
#ifdef CONFIG_MMU
    pMainTask->aspace().mapIdentity((void *)(0xfee00000), 4 * 1024);  // Identity Map APIC
#endif

    printk("APIC:\n");
    printk(" - ID:      %d\n", *pApicID >> 24);
    printk(" - Version: %d\n", *pApicVersion & 0xff);
  }
}
