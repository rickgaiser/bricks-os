#ifndef TWL_WINDOW_H
#define TWL_WINDOW_H


#include "event.h"
#include "rect.h"
#include "color.h"
#include "kernel/videoManager.h"
#include <stdint.h>


namespace twl
{


enum EBltType
{
   eBT_COPY  = 0
  ,eBT_MERGE = 1
};

class CWindow
{
public:
  CWindow();
  CWindow(int width, int height);
  virtual ~CWindow();

  // Incomming events from OS
  bool event(const CEvent & event);

  // Set application event handler
  void setEventHandler(IWindowEventHandler * eventHandler);

  // Geometry functions
  int width();
  int height();
  void rect(const CRect & rect);
  void rect(int left, int top, int width, int height);
  CRect rect();

  // Window functions
  void focus(bool focus);
  bool focus() const;
  void frame(bool focus);
  bool frame() const;

  // Drawing functions
  pixel_t pixel(int x, int y);
  void fill(color_t color);
  void fillRect(const CRect & rect, color_t color);
  void drawHLine(int y, int left, int right, color_t color);
  void drawVLine(int x, int top, int bottom, color_t color);
  void drawText(int x, int y, const char * string, color_t color = clBlack);

private:
  // Window
  IWindowEventHandler * pEventHandler_;
  CRect cWindowRect_;
  CRect cWindowOnScreenRect_;
  // Client area
  CRect cClientRect_;
  CRect cClientOnScreenRect_;

  bool bFocus_;
  bool bFrame_;

  // Display surface
  C2DRenderer * pRenderer_;
};


};


#endif
