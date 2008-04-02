#ifndef BWM_WINDOWMANAGER_H
#define BWM_WINDOWMANAGER_H


#include "window.h"
#include "kernel/videoManager.h"
#include "vector"


namespace bwm
{


class CWindowManager
{
public:
  CWindowManager();
  virtual ~CWindowManager();

  int init(CAVideoDevice * device);

  CWindow * requestNewWindow();
  int exec();

private:
  void setActive(CWindow * window);
  void drawCursor();
  void undrawCursor();

private:
  // Windows
  std::vector<CWindow *> pWindows_;
  CWindow * pActiveWindow_;

  // Cursor
  color_t underCursor_[16 * 16];
  bool bCursorActive_;
  int xOff_;
  int yOff_;

  // Display surface
  CAVideoDevice * pDevice_;
  I2DRenderer   * pRenderer_;
  CSurface      * pSurface_;
};


extern CWindowManager windowManager;


};


#endif
