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
