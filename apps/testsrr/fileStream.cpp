#include "fileStream.h"
#include "kernel/debug.h"
#include "unistd.h"
#include "fcntl.h"


//------------------------------------------------------------------------------
CFileStream::CFileStream(const char * in, const char * out)
 : pIn_(in)
 , pOut_(out)
 , fdIn_(-1)
 , fdOut_(-1)
{
}

//------------------------------------------------------------------------------
CFileStream::~CFileStream()
{
  if(fdIn_ > 0)
    close(fdIn_);

  if(fdOut_ > 0)
    close(fdOut_);
}

//------------------------------------------------------------------------------
bool
CFileStream::start()
{
  fdIn_ = open(pIn_, O_RDONLY);
  if(fdIn_ < 0)
  {
    printk("Unable to open input stream\n");
    return false;
  }

  fdOut_ = open(pOut_, O_WRONLY);
  if(fdOut_ < 0)
  {
    close(fdIn_);
    fdIn_ = -1;
    printk("Unable to open output stream\n");
    return false;
  }

  this->activate();

  return true;
}

//------------------------------------------------------------------------------
int
CFileStream::svc()
{
  char buffer[80];

  while(true)
  {
    int iSize = read(fdIn_, buffer, 80);
    if(iSize <= 0)
    {
      printk("Unable to read from input stream\n");
      return -1;
    }

    int iSize2 = write(fdOut_, buffer, iSize);
    if(iSize2 != iSize)
    {
      printk("Unable to write to output stream\n");
      return -1;
    }
  }
  return 0;
}
