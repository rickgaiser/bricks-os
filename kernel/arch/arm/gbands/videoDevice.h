#ifndef GBANDS_VIDEODEVICE_H
#define GBANDS_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"
#include "kernel/interruptManager.h"
#include "asm/arch/memory.h"


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
  virtual int isr(int irq) INTERRUPT_CODE;

  virtual void listModes(const SVideoMode ** modes, int * modeCount);
  virtual void getCurrentMode(const SVideoMode ** mode);
  virtual void getDefaultMode(const SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, int width, int height);
  virtual void get2DRenderer(I2DRenderer ** renderer);
  virtual void get3DRenderer(I3DRenderer ** renderer);

  virtual void waitVSync();
  virtual void displaySurface(CSurface * surface);

  virtual void bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy);

private:
  // Surface we're currently displaying
  CSurface * pSurface_;
  uint8_t iSurfacesFree_;

  volatile bool bSwap_;

  const SVideoMode * pCurrentMode_;
};


#endif
