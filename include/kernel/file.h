#ifndef FILE_H
#define FILE_H


#include "unistd.h"


// -----------------------------------------------------------------------------
// Linux:
// ssize_t read (struct file * file,       char * buf, size_t count, loff_t * ppos);
// ssize_t write(struct file * file, const char * buf, size_t count, loff_t * ppos);
// int     ioctl(struct inode * inode, struct file * file, unsigned int cmd, unsigned long arg);
// -----------------------------------------------------------------------------
class IFileIO
{
public:
  virtual int read (      void * data, size_t size){return(-1);}
  virtual int write(const void * data, size_t size){return(-1);}
  virtual int ioctl(unsigned int cmd, unsigned long arg){return(-1);}
};

// -----------------------------------------------------------------------------
class CFileHandle
{
public:
  CFileHandle(IFileIO * file)
   : file_(file){}

  IFileIO * file(){return(file_);}

private:
  IFileIO * file_;
};


#endif
