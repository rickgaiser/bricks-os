#ifndef DS9VIDEO_H
#define DS9VIDEO_H


#include "kernel/video.h"


class CDS9Video
 : public CAVideo
{
public:
  CDS9Video();
  virtual ~CDS9Video();

  virtual int init();

private:
  virtual void put(int iX, int iY, char c);
  virtual char get(int iX, int iY);

  unsigned short * pVideo_;
};


#endif
