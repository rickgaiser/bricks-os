#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "windowManager.h"
#include "stdlib.h"


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
        // Set default video mode
        devices[iDev]->setMode(newMode);

        // Start the window manager
        bwm::windowManager.init(devices[iDev]);
        bwm::windowManager.exec();

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
