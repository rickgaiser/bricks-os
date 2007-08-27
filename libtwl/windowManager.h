#ifndef TWL_WINDOWMANAGER_H
#define TWL_WINDOWMANAGER_H


#include "window.h"
#include <vector>


namespace twl
{


class CWindowManager
{
public:
  CWindowManager();
  virtual ~CWindowManager();

  CWindow * requestNewWindow();
  int exec();

private:
  void setActive(CWindow * window);
  void drawCursor();
  void undrawCursor();

  std::vector<CWindow *> pWindows_;
  CWindow * pActiveWindow_;
  pixel_t underCursor_[16 * 16];
  bool bCursorActive_;
  int xOff_;
  int yOff_;
};


extern CWindowManager windowManager;


};


#endif
