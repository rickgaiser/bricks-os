#ifndef TWL_WINDOWIMPL_H
#define TWL_WINDOWIMPL_H


#include "event.h"
#include "rect.h"
#include "color.h"
#include <stdint.h>


namespace twl
{


class CWindowImpl
{
public:
  CWindowImpl();
  CWindowImpl(int width, int height);
  virtual ~CWindowImpl();

  // Set application event handler
  void setEventHandler(IWindowEventHandler * eventHandler);

  void rect(const CRect & rect);
  void rect(int x, int y, int width, int height);
  CRect rect();

  // Window functions
  void frame(bool focus);
  bool frame() const;

  // Drawing functions
  void fill(color_t color);
  void fillRect(const CRect & rect, color_t color);
  void drawHLine(int x, int y, int width,  color_t color);
  void drawVLine(int x, int y, int height, color_t color);
  void drawText (int x, int y, const char * string, color_t color = clBlack);

private:
  class CPrivateData;
  CPrivateData * pPrivateData_;
};


};


#endif
