#include "fileServer.h"
#include "kernel/debug.h"
#include "kernel/srr.h"
#include "string.h"
#include "fcntl.h"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CReadServer::CReadServer(IFileIO & file)
 : ACE_Task_Base()
 , file_(file)
 , bExit_(false)
{
  // Lock the mutex so the service thread will initially block
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_mutex_lock(&mutex_);

  // Activate the service thread
  this->activate();
}

//---------------------------------------------------------------------------
CReadServer::~CReadServer()
{
  bExit_ = true;
  // Notify service thread
  k_pthread_mutex_unlock(&mutex_);
}

//---------------------------------------------------------------------------
int
CReadServer::svc()
{
  while(true)
  {
    // Wait for a request
    k_pthread_mutex_lock(&mutex_);

    if(bExit_ == true)
      break;

    // Read
    _read(iReceiveID_, size_, true);
  }

  return 0;
}

//---------------------------------------------------------------------------
void
CReadServer::read(int iReceiveID, size_t size, bool block)
{
  // Validate size
  if(size <= 0)
    msgReply(iReceiveID, -1, 0, 0);

  if(block == true)
  {
    // Save requested information
    iReceiveID_ = iReceiveID;
    size_ = size;

    // Notify service thread
    k_pthread_mutex_unlock(&mutex_);
  }
  else
  {
    // Read immediately
    _read(iReceiveID, size, false);
  }
}

//---------------------------------------------------------------------------
void
CReadServer::_read(int iReceiveID, size_t size, bool block)
{
  uint8_t * data = new uint8_t[size];
  int rv;

  rv = file_.read(data, size, block);

  if(rv > 0)
    msgReply(iReceiveID, rv, data, rv);
  else
    msgReply(iReceiveID, rv, 0, 0);

  delete data;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CWriteServer::CWriteServer(IFileIO & file)
 : ACE_Task_Base()
 , file_(file)
 , bExit_(false)
{
  // Lock the mutex so the service thread will initially block
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_mutex_lock(&mutex_);

  // Activate the service thread
  this->activate();
}

//---------------------------------------------------------------------------
CWriteServer::~CWriteServer()
{
  bExit_ = true;
  // Notify service thread
  k_pthread_mutex_unlock(&mutex_);
}

//---------------------------------------------------------------------------
int
CWriteServer::svc()
{
  while(true)
  {
    // Wait for a request
    k_pthread_mutex_lock(&mutex_);

    if(bExit_ == true)
      break;

    // Write
    _write(iReceiveID_, data_, size_, true);
  }

  return 0;
}

//---------------------------------------------------------------------------
void
CWriteServer::write(int iReceiveID, const void * data, size_t size, bool block)
{
  // Validate size
  if(size <= 0)
    msgReply(iReceiveID, -1, 0, 0);

  if(block == true)
  {
    // Save requested information
    iReceiveID_ = iReceiveID;
    data_ = data;
    size_ = size;

    // Notify service thread
    k_pthread_mutex_unlock(&mutex_);
  }
  else
  {
    // Read immediately
    _write(iReceiveID, data, size, false);
  }
}

//---------------------------------------------------------------------------
void
CWriteServer::_write(int iReceiveID, const void * data, size_t size, bool block)
{
  int rv = file_.write(data, size, block);
  msgReply(iReceiveID, rv, 0, 0);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFileServer::CFileServer(IFileIO & file, const char * name)
 : CMsgServer(name)
 , file_(file)
 , thrRead_(file)
 , thrWrite_(file)
{
  for(int i(0); i < MAX_OPEN_FILES; i++)
    flags_[i] = 0;
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

  if(iReceiveID >= MAX_OPEN_FILES)
  {
    printk("CFileServer: receive id too high %d\n", iReceiveID);
    msgReply(iReceiveID, -1, 0, 0);
    return 0;
  }

  switch(pHeader->command)
  {
    case FC_OPEN:
      flags_[iReceiveID] = ((SFileOpenHeader *)pHeader)->flags;
      break;
    case FC_CLOSE:
      flags_[iReceiveID] = 0;
      break;
    case FC_READ:
    {
      if(flags_[iReceiveID] & O_WRONLY)
      {
        printk("CFileServer: Cannot read from O_WRONLY file\n");
        msgReply(iReceiveID, -1, 0, 0);
        return 0;
      }

      thrRead_.read(iReceiveID, ((SFileReadHeader *)pHeader)->size, !(flags_[iReceiveID] & O_NONBLOCK));
      break;
    }
    case FC_WRITE:
    {
      if(flags_[iReceiveID] & O_RDONLY)
      {
        printk("CFileServer: Cannot write to O_RDONLY file\n");
        msgReply(iReceiveID, -1, 0, 0);
        return 0;
      }

      thrWrite_.write(iReceiveID, (void *)((uint8_t *)pRcvMsg + sizeof(SFileWriteHeader)), ((SFileWriteHeader *)pHeader)->size, !(flags_[iReceiveID] & O_NONBLOCK));
      break;
    }
    default:
      printk("CFileServer: Unknown command %d\n", pHeader->command);
      msgReply(iReceiveID, -1, 0, 0);
  };

  return 0;
}
