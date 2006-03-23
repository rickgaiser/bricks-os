#ifndef VIDEO_H
#define VIDEO_H


#include "file.h"


class CAVideo
 : public IFileIO
{
public:
  CAVideo(unsigned int iX, unsigned int iY);
  virtual ~CAVideo();

  // Inherited from IFileIO
  virtual int write(const void * data, size_t size);
  
  virtual void cls();
  virtual void put(char c);
  virtual void put(const char * s);

private:
  virtual void put(int iX, int iY, char c) = 0;
  virtual char get(int iX, int iY) = 0;
  virtual void scrollUp();

  unsigned int iX_;
  unsigned int iY_;

  unsigned int iCurrentX_;
  unsigned int iCurrentY_;
};


#endif
