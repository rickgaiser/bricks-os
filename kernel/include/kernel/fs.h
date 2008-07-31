#ifndef FILE_H
#define FILE_H


#include "kernel/ringBuffer.h"
#include "sys/types.h"


#define EINVAL 1


/*
// -----------------------------------------------------------------------------
// Linux file operations:
struct file_operations
{
  loff_t (*llseek) (struct file *, loff_t, int);
  ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
  ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
  int (*readdir) (struct file *, void *, filldir_t);
  int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
  int (*mmap) (struct file *, struct vm_area_struct *);
  int (*open) (struct inode *, struct file *);
  int (*flush) (struct file *, fl_owner_t id);
  int (*release) (struct inode *, struct file *);
  int (*fsync) (struct file *, struct dentry *, int datasync);
  int (*aio_fsync) (struct kiocb *, int datasync);
  int (*fasync) (int, struct file *, int);
  int (*lock) (struct file *, int, struct file_lock *);
  ssize_t (*sendfile) (struct file *, loff_t *, size_t, read_actor_t, void *);
  ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
  unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
  int (*check_flags)(int);
  int (*dir_notify)(struct file *filp, unsigned long arg);
  int (*flock) (struct file *, int, struct file_lock *);
  ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
  ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
};
*/

// -----------------------------------------------------------------------------
class IFileIO
{
public:
  virtual ~IFileIO(){}

  virtual ssize_t read (      void * buffer, size_t size, bool block = false){return -EINVAL;}
  virtual ssize_t write(const void * buffer, size_t size, bool block = false){return -EINVAL;}
//  virtual int readdir  (void *, filldir_t){return -EINVAL;}
//  virtual int ioctl    (inode *, unsigned int, unsigned long){return -EINVAL;}
//  virtual int mmap     (vm_area_struct *){return -EINVAL;}
//  virtual int open     (inode *){return -EINVAL;}
//  virtual int release  (inode *){return -EINVAL;}

//protected:
//  loff_t iOffset_;
};

// -----------------------------------------------------------------------------
class CAFileIOBufferedRead
 : public IFileIO
{
public:
  CAFileIOBufferedRead();
  virtual ~CAFileIOBufferedRead();

  virtual ssize_t read(void * buffer, size_t size, bool block = false);

protected:
  CRingBuffer bufferRead_;
};


#endif
