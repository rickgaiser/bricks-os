#include "fileServer.h"
#include "kernel/debug.h"
#include "kernel/srr.h"
#include "string.h"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFileServer::CFileServer(IFileIO & file, const char * name)
 : CMsgServer(name)
 , file_(file)
{
}

//---------------------------------------------------------------------------
CFileServer::~CFileServer()
{
}

//---------------------------------------------------------------------------
int
CFileServer::process(int iReceiveID, void * pRcvMsg)
{
  SFileHeader * pHeader = (SFileHeader *)pRcvMsg;

  switch(pHeader->command)
  {
    case FC_READ:
    {
      uint32_t rSize = ((SFileReadHeader *)pHeader)->size;
      if(rSize > 0)
      {
        uint8_t * rData = new uint8_t[rSize];

        int rv = this->read(rData, rSize);

        if(rv > 0)
          msgReply(iReceiveID, rv, rData, rv);
        else
          msgReply(iReceiveID, rv, 0, 0);

        delete rData;
      }
      else
        msgReply(iReceiveID, -1, 0, 0);
      break;
    }
    case FC_WRITE:
    {
      int rv = this->write((void *)((uint8_t *)pRcvMsg + sizeof(SFileWriteHeader)), ((SFileWriteHeader *)pHeader)->size);
      msgReply(iReceiveID, rv, 0, 0);
      break;
    }
    default:
      printk("CFileServer: Unknown command %d\n", pHeader->command);
      msgReply(iReceiveID, -1, 0, 0);
  };

  return 0;
}

//---------------------------------------------------------------------------
int
CFileServer::read(void * buffer, size_t size)
{
  return file_.read(buffer, size);
}

//---------------------------------------------------------------------------
int
CFileServer::write(const void * buffer, size_t size)
{
  return file_.write(buffer, size);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace test
{


//---------------------------------------------------------------------------
int
read(int fd, void * buffer, size_t size)
{
  int iRetVal(0);

  uint32_t iMsgSize = sizeof(SFileReadHeader);
  uint8_t * pMessage = new uint8_t[iMsgSize];
  SFileReadHeader * pHeader = (SFileReadHeader *)pMessage;

  pHeader->commonHeader.command = FC_READ;
  pHeader->size = size;

  iRetVal = msgSend(fd, pMessage, iMsgSize, buffer, size);

  delete pMessage;

  return iRetVal;
}

//---------------------------------------------------------------------------
int
write(int fd, const void * buffer, size_t size)
{
  int iRetVal(0);

  uint32_t iMsgSize = size + sizeof(SFileWriteHeader);
  uint8_t * pMessage = new uint8_t[iMsgSize];
  SFileWriteHeader * pHeader = (SFileWriteHeader *)pMessage;
  uint8_t * pData = pMessage + sizeof(SFileWriteHeader);

  pHeader->commonHeader.command = FC_WRITE;
  pHeader->size = size;
  memcpy(pData, buffer, size);

  iRetVal = msgSend(fd, pMessage, iMsgSize, 0, 0);

  delete pMessage;

  return iRetVal;
}


};
