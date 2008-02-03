#ifndef ARCH_VIDEODEVICE_H
#define ARCH_VIDEODEVICE_H


#include "kernel/video.h"
#include "kernel/videoManager.h"
#include "inttypes.h"


//---------------------------------------------------------------------------
class CPSPVideoDevice
 : public CAVideoDevice
{
public:
  CPSPVideoDevice();
  virtual ~CPSPVideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount);
  virtual void getMode(SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, ESurfaceType type);
  virtual void getRenderer(I2DRenderer ** renderer);

  virtual void waitVSync();
  virtual void displaySurface(CSurface * surface);

private:
  // Surface we're currently displaying
  CSurface * pSurface_;

  const SVideoMode    * pCurrentMode_;
};


#endif
