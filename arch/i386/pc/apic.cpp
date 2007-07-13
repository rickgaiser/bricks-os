#include "apic.h"
#include "cpuid.h"
#include "hal.h"
#include "task.h"
#include "iostream"


extern CPCTask taskMain;
uint32_t * pApicID = (uint32_t *)(0xfee00020);
uint32_t * pApicVersion = (uint32_t *)(0xfee00030);

void
init_apic()
{
  if(CPU::hasAPIC())
  {
    taskMain.aspace().identityMap((void *)(0xfee00000), 4 * 1024);  // Identity Map APIC
    
    std::cout<<"APIC:"<<std::endl;
    std::cout<<" - ID:      "<<(*pApicID >> 24)<<std::endl;
    std::cout<<" - Version: "<<(*pApicVersion & 0xff)<<std::endl;
  }
}

