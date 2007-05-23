#ifndef I386VIDEO_H
#define I386VIDEO_H


#include "kernel/video.h"


#define CHARS_PER_LINE 80
#define LINE_COUNT     25
#define VIDEO_START    ((char *)0xB8000)


class CI386Video
 : public CAVideo
{
public:
  CI386Video();
  virtual ~CI386Video();

  virtual int init();

private:
  virtual void put(int iX, int iY, char c);
  virtual char get(int iX, int iY);

  char * pVideo_;
};


#endif
