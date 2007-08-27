#ifndef TWL_PIXMAP_H
#define TWL_PIXMAP_H


#include "window.h"
#include "rect.h"
//#include <stdint.h>


namespace twl
{


class CPixmap
{
public:
  CPixmap();
  CPixmap(int width, int height);
  CPixmap(const char * filename);
  virtual ~CPixmap();

  pixel_t & pixel(int x, int y);
  int width();
  int height();
  void fill(color_t color);
  void fillRect(CRect & rect, color_t color);
  void drawHLine(int y, int left, int right, color_t color);
  void drawVLine(int x, int top, int bottom, color_t color);
  void bitBlt(int x, int y, CPixmap & pixmap, EBltType type = eBT_COPY);
  void load(const char * filename);

  pixel_t * pData_;
private:
  int iWidth_;
  int iHeight_;
};


};


#endif
