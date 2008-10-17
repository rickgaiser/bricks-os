#ifndef KERNEL_2DRENDERER_H
#define KERNEL_2DRENDERER_H


#include "inttypes.h"
#include "kernel/videoManager.h"


//---------------------------------------------------------------------------
class I2DRenderer
 : public IRenderer
{
public:
  virtual ~I2DRenderer(){}

  // Color
  virtual void       setColor(color_t rgb) = 0; // cfA8R8G8B8 format color
  virtual void       setColor(uint8_t r, uint8_t g, uint8_t b) = 0;

  // Drawing
  virtual void       setPixel(int x, int y) = 0;
  virtual void       fill() = 0;
  virtual void       fillRect(int x, int y, unsigned int width, unsigned int height) = 0;  // Width: width in pixels, Height: height in pixels
  virtual void       drawLine(int x1, int y1, int x2, int y2) = 0;                         // Note: x2 and y2 are drawn
  virtual void       drawHLine(int x, int y, unsigned int width) = 0;
  virtual void       drawVLine(int x, int y, unsigned int height) = 0;
  virtual void       drawRect(int x, int y, unsigned int width, unsigned int height) = 0;
};

//---------------------------------------------------------------------------
class C2DRenderer
 : public I2DRenderer
{
public:
  C2DRenderer(CSurface * surf = 0);
  virtual ~C2DRenderer();

  // Surfaces
  virtual void       setSurface(CSurface * surface);
  virtual CSurface * getSurface();

  // Flush operations to surface
  virtual void       flush();

  // Color
  virtual void       setColor(color_t rgb); // cfA8R8G8B8 format color
  virtual void       setColor(uint8_t r, uint8_t g, uint8_t b);

  // Drawing
  virtual void       setPixel(int x, int y);
  virtual void       fill();
  virtual void       fillRect(int x, int y, unsigned int width, unsigned int height);
  virtual void       drawLine(int x1, int y1, int x2, int y2);
  virtual void       drawHLine(int x, int y, unsigned int width);
  virtual void       drawVLine(int x, int y, unsigned int height);
  virtual void       drawRect(int x, int y, unsigned int width, unsigned int height);

protected:
  // Drawing
  virtual void       setPixel_i(int x, int y);
  virtual void       fill_i();
  virtual void       fillRect_i(int x, int y, unsigned int width, unsigned int height);
  virtual void       drawLine_i(int x1, int y1, int x2, int y2);
  virtual void       drawHLine_i(int x, int y, unsigned int width);
  virtual void       drawVLine_i(int x, int y, unsigned int height);
  virtual void       drawRect_i(int x, int y, unsigned int width, unsigned int height);

protected:
  // Surface we're currently rendering on
  CSurface * pSurface_;

  // Current drawing color
  SColor color_;      // Color
  color_t fmtColor_;  // Pre calculated surface format color
};


#endif
