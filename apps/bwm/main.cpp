#include "asm/arch/config.h"
#include "kernel/debug.h"
#include "kernel/videoManager.h"
#include "../../libtwl/windowManager.h"
#include "../../libtwl/application.h"
#include "../../libtwl/desktop.h"
#include "stdlib.h"


// -----------------------------------------------------------------------------
int
guiMain(int argc, char * argv[])
{
  twl::CApplication app(argc, argv);
  twl::CDesktop desktop;

  desktop.frame(false);         // Desktop has no window frame
  desktop.rect(0, 0, 240, 160); // FIXME: Size depends on screen resolution
  desktop.visible(true);        // Show desktop

  return app.exec();
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

        twl::windowManager.init(devices[iDev]);

        guiMain(argc, argv);

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
