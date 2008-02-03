#ifndef ARCH_DEBUGSCREEN_H
#define ARCH_DEBUGSCREEN_H


#include "kernel/video.h"


#define CHARS_PER_LINE 80
#define LINE_COUNT     25
#define VIDEO_START    ((char *)0xB8000)


class CI386DebugScreen
 : public CAVideo
{
public:
  CI386DebugScreen();
  virtual ~CI386DebugScreen();

  virtual int init();

private:
  virtual void put(int iX, int iY, char c);
  virtual char get(int iX, int iY);

  char * pVideo_;
};


#endif
