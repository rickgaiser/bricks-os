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
    bool bDone(false);
    while(bDone == false)
    {
      uint8_t slaves;

      printk("Detecting slave for multiboot\n");
      printk("Press A to begin\n");
      while(true)
      {
        // Locate slaves
        slaves = cSerial.locateMultiBootSlaves();
        printk("\rDetected slaves: ");
        if(slaves & (1<<1)) printk("1 ");
        if(slaves & (1<<2)) printk("2 ");
        if(slaves & (1<<3)) printk("3 ");

        // Wait
        for(vuint32_t i(0); i < 100000; i++)
          if((getInput() & KEY_A) && (slaves != 0))
            break;
        if((getInput() & KEY_A) && (slaves != 0))
          break;
      }
      printk("\n");

      printk("Booting slaves\n");
      int status = cSerial.multiBoot();
      if(status == 0)
        bDone = true;
      else
        printk("failed (%d)\n", status);
    }
  }

  printk("ready\n");

  return 0;
}
