#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "stdlib.h"


extern void testGL(CAVideoDevice * device, CSurface * surface_a, CSurface * surface_b);


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
      const SVideoMode * modes;
      int iModeCount;
      devices[iDev]->listModes(&modes, &iModeCount);
      if(iModeCount > 0)
      {
        for(int iMode(0); iMode < iModeCount; iMode++)
        {
          devices[iDev]->setMode(&modes[iMode]);
          CSurface * pVideoSurfaceA;
          CSurface * pVideoSurfaceB;

          // Test 3d with a double buffered surface
          devices[iDev]->getSurface(&pVideoSurfaceA, stSCREEN);
          devices[iDev]->getSurface(&pVideoSurfaceB, stOFFSCREEN);
          devices[iDev]->displaySurface(pVideoSurfaceA);
          testGL(devices[iDev], pVideoSurfaceA, pVideoSurfaceB);

          // Clean up allocated surfaces
          delete pVideoSurfaceB;
          delete pVideoSurfaceA;
        }
      }
      else
      {
        printk("ERROR: Device has no modes!\n");
      }
    }
  }
  else
  {
    printk("ERROR: No video devices!\n");
  }

  return 0;
}
