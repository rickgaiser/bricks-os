#ifndef KERNEL_SRRCHANNEL_H
#define KERNEL_SRRCHANNEL_H


#include "kernel/syscall.h"
#include "kernel/fs.h"
#include "sys/types.h"
#include "inttypes.h"


#define BUFFER_SIZE 100


// -----------------------------------------------------------------------------
enum EChannelState
{
  CS_FREE,
  CS_USED,
  CS_MSG_SEND,
  CS_MSG_RECEIVED,
  CS_MSG_REPLIED
};

// -----------------------------------------------------------------------------
class IChannelClient
{
public:
  IChannelClient(){}
  virtual ~IChannelClient(){}

  virtual int msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize) = 0;
};

// -----------------------------------------------------------------------------
class IChannelServer
{
public:
  IChannelServer(){}
  virtual ~IChannelServer(){}

  virtual int msgReceive(int iChannelID, void * pRcvMsg, int iRcvSize) = 0;
  virtual int msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize) = 0;
};

// -----------------------------------------------------------------------------
class CChannel
 : public IChannelClient
 , public IChannelServer
{
public:
  CChannel();
  virtual ~CChannel();

  virtual int msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);
  virtual int msgReceive(int iChannelID, void * pRcvMsg, int iRcvSize);
  virtual int msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize);

private:
  pthread_mutex_t mutex_;       // The big channel lock
  pthread_cond_t stateCond_;    // State change condition
  int iState_;
  const void * pSndMsg_;
  int iSndSize_;
  void * pRcvMsg_;
  int iRcvSize_;
  int iRetVal_;
};

// -----------------------------------------------------------------------------
class CRemoteConnection
 : public IChannelClient
 , public IFileIO
{
public:
  CRemoteConnection(IFileIO & channel);
  virtual ~CRemoteConnection();

  virtual int msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);

  // Callback handler for tunnel
  virtual ssize_t write(const void * buffer, size_t size, loff_t * = 0);

private:
  // Reply message through tunnel
  virtual int msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize);

  // Buffer for incomming data from tunnel
  uint8_t buffer_[BUFFER_SIZE];
  uint32_t iBufferIndex_;

  IFileIO  & tunnel_;
  uint32_t   iMsgID_;

  // Locking
  pthread_mutex_t mutex_;
  pthread_cond_t stateCond_;
  int iState_;

  // Reply information
  uint8_t * pRcvMsg_;
  uint32_t iRcvSize_;
  int iStatus_;
};


#ifdef __cplusplus
extern "C" {
#endif


// Channels
declareSysCallKernel1r(int, channelCreate, unsigned, iFlags);
declareSysCallKernel1r(int, channelDestroy, int, iChannelID);
declareSysCallKernel4r(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
declareSysCallKernel1r(int, channelConnectDetach, int, iConnectionID);


#ifdef __cplusplus
}
#endif


#endif
