#ifndef GBANDS_VIDEODEVICE_H
#define GBANDS_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/interruptManager.h"


//---------------------------------------------------------------------------
class CGBASurface
 : public CSurface
{
public:
  CGBASurface();
  virtual ~CGBASurface();

  // Fill entire surface
  virtual void fill();
  // Fill rect on surface
  virtual void fillRect(int x, int y, int width, int height);

  // Swap back buffer to front buffer, only if back buffer exists
  virtual void     swap(bool sync = false);

  // Wait for Vertical Synchronization
  virtual void     waitVSync();
};

//---------------------------------------------------------------------------
class CGBAVideoDevice
 : public CAVideoDevice
 , public IInterruptServiceRoutine
{
public:
  CGBAVideoDevice();
  virtual ~CGBAVideoDevice();

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

  virtual void listModes(const SVideoMode ** modes, int * modeCount);
  virtual void getMode(SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, ESurfaceType type);

private:
  const SVideoMode * pCurrentMode_;
};


#endif
