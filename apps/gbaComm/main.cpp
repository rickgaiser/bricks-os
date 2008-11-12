/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "asm/arch/config.h"
#include "asm/arch/registers.h"
#include "kernel/debug.h"
#include "stdlib.h"
#include "../../kernel/arch/arm/gbands/gbaSerial.h"


extern CGBASerial cSerial;
#define getInput() (~REG_KEYS)


// -----------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  if(cSerial.getDeviceID() == 0)
  {
    bool bDone(false);
    int iRun(0);
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
        switch(iRun & 0x3)
        {
          case 0: printk("-"); break;
          case 1: printk("\\"); break;
          case 2: printk("|"); break;
          case 3: printk("/"); break;
        };
        iRun++;

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
