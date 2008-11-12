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
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "stdlib.h"

#include "raytracer.h"


// -----------------------------------------------------------------------------
void
testRaytracer(CAVideoDevice * device, CSurface * surface)
{
  if(surface == NULL)
    return;

  CRaytracer tracer;
  tracer.setSurface(surface);

  while(true)
  {
    tracer.render();
  }
}

// -----------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  CAVideoDevice ** devices;
  int iDeviceCount;
  videoManager.listDevices(&devices, &iDeviceCount);
  if(iDeviceCount > 0)
  {
    for(int iDev(0); iDev < iDeviceCount; iDev++)
    {
      const SVideoMode * mode;
      devices[iDev]->getDefaultMode(&mode);
      devices[iDev]->setMode(mode);
      CSurface    * pVideoSurface;

      // Test raytracing with a single buffered surface
      devices[iDev]->getSurface(&pVideoSurface, mode->width, mode->height);
      devices[iDev]->displaySurface(pVideoSurface);
      testRaytracer(devices[iDev], pVideoSurface);

      if(pVideoSurface != NULL)
        delete pVideoSurface;
    }
  }
  else
  {
    printk("ERROR: No video devices!\n");
  }

  return 0;
}
