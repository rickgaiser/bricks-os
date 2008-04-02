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
    // Test all devices (normally there is only one)
    for(int iDev(0); iDev < iDeviceCount; iDev++)
    {
      const SVideoMode * prevMode = NULL;
      const SVideoMode * newMode  = NULL;
      devices[iDev]->getCurrentMode(&prevMode);
      devices[iDev]->getDefaultMode(&newMode);
      if(newMode != NULL)
      {
        CSurface * pVideoSurfaceA = NULL;
        CSurface * pVideoSurfaceB = NULL;

        // Set default video mode
        devices[iDev]->setMode(newMode);

        // Allocate two surfaces for double buffering
        devices[iDev]->getSurface(&pVideoSurfaceA, newMode->width, newMode->height);
        devices[iDev]->getSurface(&pVideoSurfaceB, newMode->width, newMode->height);

        // Start test
        if((pVideoSurfaceA != NULL) && (pVideoSurfaceB != NULL))
          testGL(devices[iDev], pVideoSurfaceA, pVideoSurfaceB);

        // Clean up allocated surfaces
        if(pVideoSurfaceB != NULL)
          delete pVideoSurfaceB;
        if(pVideoSurfaceA != NULL)
          delete pVideoSurfaceA;

        // Restore video mode
        if(prevMode != NULL)
          devices[iDev]->setMode(prevMode);
      }
      else
        printk("ERROR: Device has no modes!\n");
    }
  }
  else
    printk("ERROR: No video devices!\n");

  return 0;
}
