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


#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include "asm/arch/config.h"
#include "kernel/bricks.h"
#include "kernel/debug.h"
//#include "kernel/memoryManager.h"

#ifdef CONFIG_DEBUGGING
#include "debugScreen.h"
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
#include "videoDevice.h"
#endif // CONFIG_FRAMEBUFFER


#ifdef CONFIG_DEBUGGING
CPSPDebugScreen cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
CPSPVideoDevice * pVideoDevice;
#endif // CONFIG_FRAMEBUFFER


PSP_MODULE_INFO("Bricks-OS", 0, 1, 1);


// -----------------------------------------------------------------------------
int
exit_callback(int arg1, int arg2, void *common)
{
  sceKernelExitGame();

  return 0;
}

// -----------------------------------------------------------------------------
int
CallbackThread(SceSize args, void *argp)
{
  int cbid;

  cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
  sceKernelRegisterExitCallback(cbid);

  sceKernelSleepThreadCB();

  return 0;
}

// -----------------------------------------------------------------------------
int
SetupCallbacks(void)
{
  int thid = 0;

  thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
  if(thid >= 0)
  {
    sceKernelStartThread(thid, 0, 0);
  }

  return thid;
}

// -----------------------------------------------------------------------------
int
main(int, char *[])
{
  int iRetVal(0);

  //init_heap((void *)HEAP_START, HEAP_END - HEAP_START);

  SetupCallbacks();

#ifdef CONFIG_DEBUGGING
  if(cDebug.init() == -1)
    iRetVal = -1;
  pDebug = &cDebug;
#endif // CONFIG_DEBUGGING

#ifdef CONFIG_FRAMEBUFFER
  pVideoDevice = new CPSPVideoDevice;
#endif // CONFIG_FRAMEBUFFER

//  sceKernelSleepThread();

  //printk("heap: %dKiB\n", (HEAP_END - HEAP_START) / 1024);

  printk("PSP arch ready\n");

  return bricks_main();
}
