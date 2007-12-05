#ifndef PS2_VIDEODEVICE_H
#define PS2_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/fs.h"


#define TEXT_WIDTH 45
#define TEXT_HEIGHT 17


//---------------------------------------------------------------------------
struct SPS2VideoMode
{
  uint16_t width;
  uint16_t height;
  uint16_t bpp;
  uint16_t mode;
  uint16_t psm;
  uint16_t interlace;
  uint16_t field;
  uint64_t display;
};


class CPS2VideoDevice;
//---------------------------------------------------------------------------
class CPS2Surface
 : public CSurface
{
public:
  CPS2Surface();
  virtual ~CPS2Surface();

public:
  uint16_t psm_;
};

//---------------------------------------------------------------------------
class CPS22DRenderer
 : public I2DRenderer
{
public:
  CPS22DRenderer(CSurface * surf = 0);
  virtual ~CPS22DRenderer();

  // Surfaces
  virtual void       setSurface(CSurface * surf);
  virtual CSurface * getSurface();

  // Color
  virtual void       setColor(uint8_t r, uint8_t g, uint8_t b);

  // Drawing
  virtual void       setPixel(int x, int y);
  virtual void       fill();
  virtual void       fillRect(int x, int y, unsigned int width, unsigned int height);
  virtual void       drawLine(int x1, int y1, int x2, int y2);
  virtual void       drawRect(int x, int y, unsigned int width, unsigned int height);

private:
  // Surface we're currently rendering on
  CPS2Surface * pSurface_;

  // Current drawing color
  SColor color_;
};

//---------------------------------------------------------------------------
class CPS2VideoDevice
 : public CAVideoDevice
{
public:
  CPS2VideoDevice();
  virtual ~CPS2VideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount);
  virtual void getMode(SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, ESurfaceType type);
  virtual void getRenderer(I2DRenderer ** renderer);

private:
  const SVideoMode    * pCurrentMode_;
  const SPS2VideoMode * pCurrentPS2Mode_;
};


#endif
