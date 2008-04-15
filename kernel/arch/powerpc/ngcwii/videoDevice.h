#ifndef ARCH_VIDEODEVICE_H
#define ARCH_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"
#include "../../../../gl/softGLF.h"


//---------------------------------------------------------------------------
class CNGCVideoDevice;

//---------------------------------------------------------------------------
class CNGC2DRenderer
 : public C2DRenderer
{
public:
  CNGC2DRenderer(CNGCVideoDevice * dev);
  virtual ~CNGC2DRenderer();

  // Flush operations to surface
  virtual void flush();

  // Color
  virtual void setColor(color_t rgb);
  virtual void setColor(uint8_t r, uint8_t g, uint8_t b);

private:
  CNGCVideoDevice * pDev_;
};

//-----------------------------------------------------------------------------
class CNGC3DRenderer
 : public CSoftGLESFloat
{
public:
  CNGC3DRenderer(CNGCVideoDevice * dev);
  virtual ~CNGC3DRenderer();

  // Flush operations to surface
  virtual void flush();
  virtual void glFlush(){flush();}

private:
  CNGCVideoDevice * pDev_;
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

  // Flush RGB buffer to native screen
  void flush(CSurface * surface);

  void setHorizontal(uint16_t xoffset, uint16_t width);
  void setVertical  (uint16_t yoffset, uint16_t height);
  void setInterlaced(bool interlaced);

private:
  // Surface we're currently displaying
  CSurface * pSurface_;

  // Current Native surface (RGB mode only)
  void * pNativeSurface_;

  const SVideoMode * pCurrentMode_;
};


#endif
