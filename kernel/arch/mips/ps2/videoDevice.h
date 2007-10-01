#ifndef PS2_VIDEODEVICE_H
#define PS2_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/fs.h"


#define TEXT_WIDTH 45
#define TEXT_HEIGHT 17


//---------------------------------------------------------------------------
class CPS2Surface
 : public CSurface
 , public IFileIO
{
public:
  CPS2Surface();
  virtual ~CPS2Surface();

  // Inherited from IFileIO
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);

  virtual void     setColor(uint8_t r, uint8_t g, uint8_t b);
  virtual void     setFillColor(uint8_t r, uint8_t g, uint8_t b);

  virtual void     setPixel(int x, int y);
  virtual void     fill();
  virtual void     fillRect(int x, int y, int width, int height);
  virtual void     drawLine(int x1, int y1, int x2, int y2);
  virtual void     drawRect(int x, int y, int width, int height);

  // Swap back buffer to front buffer, only if back buffer exists
  virtual void swap(bool bForceCopy = false);

private:
  unsigned int iCurrentX_;
  unsigned int iCurrentY_;

  char pBuffer_[TEXT_HEIGHT][TEXT_WIDTH];
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

private:
  const SVideoMode * pCurrentMode_;
};


#endif
