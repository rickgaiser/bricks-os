#ifndef KERNEL_SRRCHANNEL_H
#define KERNEL_SRRCHANNEL_H


#include "kernel/syscall.h"
#include "kernel/fs.h"
#include "sys/types.h"
#include "inttypes.h"


#define MAX_IN_CONNECTION_COUNT 10


// Create a receive id from a channel and a connection id
#define RCVID_CREATE(chid,coid) (((chid)<<16)||(coid))
// Get channel id from receive id
#define RCVID_GET_CHID(rcvid) (((rcvid)>>16)&0xffff)
// Get connection id from receive id
#define RCVID_GET_COID(rcvid) ((rcvid)&0xffff)


// -----------------------------------------------------------------------------
enum EChannelState
{
  CHS_FREE,
  CHS_MSG_SENT,
};

// -----------------------------------------------------------------------------
enum EConnectionState
{
  COS_FREE,
  COS_MSG_SENT,
  COS_MSG_REPLIED,
};

// -----------------------------------------------------------------------------
// Early declarations
class CChannel;
class CConnection;

//------------------------------------------------------------------------------
class CChannel
{
public:
  CChannel();
  virtual ~CChannel();

  void msgSend(CConnection & connection);
  int msgReceive(void * pRcvMsg, int iRcvSize);
  int msgReply(int iConnectionIDX, int iStatus, const void * pReplyMsg, int iReplySize);

//private:
  // Connections to this channels (CTask has ownership!)
  // NOTE: We are using an array here becouse it is FAST, and we need the
  //       msgReply function to locate connections FAST.
  CConnection * pConnectionsIn_[MAX_IN_CONNECTION_COUNT]; // Queue to hold all of the connections to this channel

private:
  CConnection * pMsgWaiting_;   // Connection that has a message waiting

  pthread_mutex_t mutex_;       // The channel lock
  pthread_cond_t stateCond_;    // State change condition
  int iState_;                  // Current state
};

// -----------------------------------------------------------------------------
class CConnection
{
  friend class CChannel;

public:
  CConnection(CChannel * channel, int iReceiveID);
  virtual ~CConnection();

  int msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);
  int msgReply(int iStatus, const void * pReplyMsg, int iReplySize);

private:
  CChannel * channel_;
  int iReceiveID_;              // msgReceive will return this id

  pthread_mutex_t mutex_;       // Mutex to notify us of a replied message
  pthread_cond_t stateCond_;    // State change condition
  int iState_;                  // Current state

  const void * pSndMsg_;        // Location of the sent message
  int iSndSize_;                // Size of the sent message
  void * pRcvMsg_;              // Location where return message can be stored
  int iRcvSize_;                // Size of the return message buffer
  int iRetVal_;                 // Return value for the msgSend function
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
