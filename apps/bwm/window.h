#ifndef BWM_WINDOW_H
#define BWM_WINDOW_H


#include "event.h"
#include "rect.h"
#include "color.h"
#include "kernel/videoManager.h"
#include <stdint.h>


namespace bwm
{


enum EBltType
{
   eBT_COPY  = 0
  ,eBT_MERGE = 1
};

class CWindow
 : public I2DRenderer
{
public:
  CWindow(I2DRenderer * renderer);
  virtual ~CWindow();

  // Incomming events from OS
  bool event(const twl::CEvent & event);

  // Set application event handler
  void setEventHandler(twl::IWindowEventHandler * eventHandler);

  // Geometry functions
  int width();
  int height();
  void rect(const twl::CRect & rect);
  void rect(int x, int y, int width, int height);
  twl::CRect rect();

  // Window functions
  void focus(bool focus);
  bool focus() const;
  void frame(bool focus);
  bool frame() const;

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

private:
  // Window
  twl::IWindowEventHandler * pEventHandler_;
  twl::CRect cWindowRect_;
  twl::CRect cWindowOnScreenRect_;
  // Client area
  twl::CRect cClientRect_;
  twl::CRect cClientOnScreenRect_;

  bool bNeedRedraw_;
  bool bFocus_;
  bool bFrame_;

  // Display surface
  I2DRenderer * pRenderer_;
};


};


#endif
