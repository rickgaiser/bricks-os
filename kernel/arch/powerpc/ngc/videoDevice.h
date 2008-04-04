#ifndef ARCH_VIDEODEVICE_H
#define ARCH_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"


//---------------------------------------------------------------------------
class CNGCSurface
 : public CSurface
{
public:
  CNGCSurface();
  virtual ~CNGCSurface();

public:
  void * pn;
};

//---------------------------------------------------------------------------
class CNGC2DRenderer
 : public C2DRenderer
{
public:
  CNGC2DRenderer();
  virtual ~CNGC2DRenderer();

  // Flush operations to surface
  virtual void flush();

  // Color
  virtual void setColor(color_t rgb);
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

  const SVideoMode * pCurrentMode_;
};


#endif
