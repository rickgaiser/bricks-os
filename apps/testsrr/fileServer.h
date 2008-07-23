#ifndef FILESERVER_H
#define FILESERVER_H


#include "msgServer.h"
#include "kernel/fs.h"


//---------------------------------------------------------------------------
enum EFileCommand
{
  FC_READ  = 1,
  FC_WRITE = 2,
};

struct SFileHeader
{
  uint32_t command;
};

struct SFileReadHeader
{
  SFileHeader commonHeader;
  uint32_t size;
};

struct SFileWriteHeader
{
  SFileHeader commonHeader;
  uint32_t size;
};


//---------------------------------------------------------------------------
class CFileServer
 : public CMsgServer
{
public:
  CFileServer(IFileIO & file, const char * name);
  virtual ~CFileServer();

  virtual int process(int iReceiveID, void * pRcvMsg);

  virtual int read (      void * buffer, size_t size);
  virtual int write(const void * buffer, size_t size);

private:
  IFileIO & file_;
};


//---------------------------------------------------------------------------
namespace test
{
  int read (int fd,       void * buffer, size_t size);
  int write(int fd, const void * buffer, size_t size);
};


#endif
