#include "apic.h"
#include "cpuid.h"
#include "hal.h"
#include "task.h"
#include "kernel/debug.h"


extern CPCThread * pMainThread;
uint32_t * pApicID = (uint32_t *)(0xfee00020);
uint32_t * pApicVersion = (uint32_t *)(0xfee00030);

void
init_apic()
{
  if(CPU::hasAPIC())
  {
    pMainThread->aspace().identityMap((void *)(0xfee00000), 4 * 1024);  // Identity Map APIC

    printk("APIC:\n");
    printk(" - ID:      %d\n", *pApicID >> 24);
    printk(" - Version: %d\n", *pApicVersion & 0xff);
  }
}

