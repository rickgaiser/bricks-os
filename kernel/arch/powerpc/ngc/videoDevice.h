#ifndef ARCH_VIDEODEVICE_H
#define ARCH_VIDEODEVICE_H


#include "kernel/videoManager.h"


//---------------------------------------------------------------------------
class CNGC2DRenderer
 : public C2DRenderer
{
public:
  CNGC2DRenderer();
  virtual ~CNGC2DRenderer();

  // Color
  virtual void setColor(uint8_t r, uint8_t g, uint8_t b);
};

//---------------------------------------------------------------------------
class CNGCVideoDevice
 : public CAVideoDevice
{
public:
  CNGCVideoDevice();
  virtual ~CNGCVideoDevice();

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

  const SVideoMode * pCurrentMode_;
};


#endif
