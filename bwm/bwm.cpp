#include "bwm/bwm.h"
#include "kernel/videoManager.h"
#include "iostream"


// Default desktop color
color_t clDesktop(BxRGB(58, 110, 165));
// Window title-bar colors
color_t clActiveWindowL(BxRGB(10, 36, 106));
color_t clActiveWindowR(BxRGB(166, 202, 240));
color_t clActiveWindow(clActiveWindowL);
color_t clInActiveWindowL(BxRGB(128, 128, 128));
color_t clInActiveWindowR(BxRGB(192, 192, 192));
color_t clInActiveWindow(clInActiveWindowL);
// Panel colors
color_t clPanelLight(BxRGB(255, 255, 255));
color_t clPanelShade1(BxRGB(64, 64, 64));
color_t clPanelShade2(BxRGB(128, 128, 128));
color_t clPanelFill(BxRGB(212, 208, 200));


// -----------------------------------------------------------------------------
void
bwm()
{
  CAVideoDevice ** devices;
  int iDeviceCount;
  videoManager.listDevices(&devices, &iDeviceCount);
  std::cout<<"Found "<<iDeviceCount<<" video device(s)"<<std::endl;
  for(int iDev(0); iDev < iDeviceCount; iDev++)
  {
    const SVideoMode * modes;
    int iModeCount;
    devices[iDev]->listModes(&modes, &iModeCount);
    std::cout<<" - dev1: "<<iModeCount<<" mode(s)"<<std::endl;
    for(int iMode(0); iMode < iModeCount; iMode++)
    {
      std::cout<<"   - "<<modes[iMode].xres<<"x"<<modes[iMode].yres<<"x"<<modes[iMode].bitsPerPixel<<std::endl;
      if((modes[iMode].format == pfR5G5B5) || (modes[iMode].format == pfA1R5G5B5))
      {
        devices[iDev]->setMode(&modes[iMode]);
        CSurface * pVideoSurface;
        devices[iDev]->getSurface(&pVideoSurface, stSCREEN);
        pVideoSurface->fill(clDesktop);
        pVideoSurface->swap();
        for(int i(0); i < 0x7fff; i++)
        {
          pVideoSurface->fill(i | 0x8000);
          pVideoSurface->swap(true);
        }
        delete pVideoSurface;
      }
    }
  }
}
