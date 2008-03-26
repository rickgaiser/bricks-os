#ifndef GBANDS_VIDEODEVICE_H
#define GBANDS_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/interruptManager.h"


//---------------------------------------------------------------------------
class CGBA2DRenderer
 : public C2DRenderer
{
public:
  CGBA2DRenderer();
  virtual ~CGBA2DRenderer();

protected:
  virtual void fill_i();
  virtual void fillRect_i(int x, int y, unsigned int width, unsigned int height);
  virtual void drawHLine_i(int x, int y, unsigned int width);
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
  virtual void getCurrentMode(const SVideoMode ** mode);
  virtual void getDefaultMode(const SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, ESurfaceType type);
  virtual void getRenderer(I2DRenderer ** renderer);

  virtual void waitVSync();
  virtual void displaySurface(CSurface * surface);

private:
  // Surface we're currently displaying
  CSurface * pSurface_;

  volatile bool bSwap_;

  const SVideoMode * pCurrentMode_;
};


#endif
