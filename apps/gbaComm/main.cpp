#include "asm/arch/config.h"
#include "asm/arch/registers.h"
#include "kernel/debug.h"
#include "stdlib.h"
#include "../../kernel/arch/arm/gbands/gbaSerial.h"


extern CGBASerial cSerial;
uint16_t keys(0);


// -----------------------------------------------------------------------------
uint16_t
getInput()
{
  return ~REG_KEYS;
}

// -----------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  if(cSerial.getDeviceID() == 0)
  {
    uint8_t slaves;

    printk("Detecting slave for multiboot\n");
    printk("Press A to begin\n");
    while(1)
    {
      // Locate slaves
      slaves = cSerial.locateMultiBootSlaves();
      printk("\rDetected slaves: ");
      if(slaves & 0x02) printk("1 ");
      if(slaves & 0x04) printk("2 ");
      if(slaves & 0x08) printk("3 ");

      // Wait
      for(vuint32_t i(0); i < 100000; i++)
        if(getInput() & KEY_A)
          break;
      if(getInput() & KEY_A)
        break;
    }
    printk("\n");

    if(slaves != 0)
    {
      printk("Booting slaves\n");
      cSerial.multiBoot();
      /*
      printk("Press B to abort\n");
      while(cSerial.multiBoot() == -1)
      {
        // Wait
        for(vuint32_t i(0); i < 100000; i++);
          if(getInput() & KEY_B)
            break;
        if(getInput() & KEY_B)
          break;
      }
      */
    }
  }

  printk("ready\n");

  return 0;
}
