#ifndef FILESERVER_H
#define FILESERVER_H


#include "msgServer.h"
#include "kernel/fs.h"
#include "kernel/fileDriver.h"


//---------------------------------------------------------------------------
class CReadServer
 : public ACE_Task_Base
{
public:
  CReadServer(IFileIO & file);
  virtual ~CReadServer();

  int svc();
  void read(int iReceiveID, size_t size, bool block);

private:
  void _read(int iReceiveID, size_t size, bool block);

  IFileIO & file_;
  volatile int iReceiveID_;
  volatile size_t size_;
  pthread_mutex_t mutex_;
  volatile bool bExit_;
};

//---------------------------------------------------------------------------
class CWriteServer
 : public ACE_Task_Base
{
public:
  CWriteServer(IFileIO & file);
  virtual ~CWriteServer();

  int svc();
  void write(int iReceiveID, const void * data, size_t size, bool block);

private:
  void _write(int iReceiveID, const void * data, size_t size, bool block);

  IFileIO & file_;
  volatile int iReceiveID_;
  const void * data_;
  volatile size_t size_;
  pthread_mutex_t mutex_;
  volatile bool bExit_;
};

//---------------------------------------------------------------------------
// Our user space file server needs 3 threads:
//  1 - Process incomming messages      (CMsgServer)
//  2 - Process blocking read requests  (CReadServer)
//  3 - Process blocking write requests (CWriteServer)
class CFileServer
 : public CMsgServer
{
public:
  CFileServer(IFileIO & file, const char * name);
  virtual ~CFileServer();

  virtual int process(int iReceiveID, void * pRcvMsg);

private:
  IFileIO & file_;
  CReadServer thrRead_;
  CWriteServer thrWrite_;
  uint32_t flags_[MAX_OPEN_FILES];
};


#endif
