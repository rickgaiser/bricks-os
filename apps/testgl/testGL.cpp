#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "kernel/3dRenderer.h"
#include "stdlib.h"
#include "glconfig.h"


#ifdef USE_FLOATING_POINT
extern void testGLF (CAVideoDevice * device, CSurface * surface_a, CSurface * surface_b);
#else
extern void testGLFx(CAVideoDevice * device, CSurface * surface_a, CSurface * surface_b);
#endif


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
#ifdef ENABLE_DOUBLE_BUFFERED
        devices[iDev]->getSurface(&pVideoSurfaceB, newMode->width, newMode->height);
#endif

        // Start test
        if(
           (pVideoSurfaceA != NULL)
#ifdef ENABLE_DOUBLE_BUFFERED
        && (pVideoSurfaceB != NULL)
#endif
          )
        {
#ifdef USE_FLOATING_POINT
          testGLF (devices[iDev], pVideoSurfaceA, pVideoSurfaceB);
#else
          testGLFx(devices[iDev], pVideoSurfaceA, pVideoSurfaceB);
#endif
        }

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
