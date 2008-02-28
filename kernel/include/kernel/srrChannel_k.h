#ifndef KERNEL_SRRCHANNEL_H
#define KERNEL_SRRCHANNEL_H


#include "kernel/syscall.h"
#include "kernel/fs.h"
#include "sys/types.h"
#include "inttypes.h"


#define MAX_IN_CONNECTION_COUNT 10


// -----------------------------------------------------------------------------
enum EChannelState
{
  CHS_FREE,
  CHS_MSG_SENT,
};

// -----------------------------------------------------------------------------
// Early declarations
class CConnection;

//------------------------------------------------------------------------------
class CChannel
{
public:
  CChannel();
  virtual ~CChannel();

  int  addConnection(CConnection * connection);
  void removeConnection(CConnection * connection);

  void msgSend(CConnection & connection);
  int  msgReceive(void * pRcvMsg, int iRcvSize);

private:
  // Connections to this channels (CTask has ownership!)
  // NOTE: We are using an array here becouse it is FAST, and we need the
  //       msgReply function to locate connections FAST.
  CConnection * pConnectionsIn_[MAX_IN_CONNECTION_COUNT];
  CConnection * pMsgWaiting_;   // Connection that has a message waiting

  pthread_mutex_t mutex_;       // The channel lock
  pthread_cond_t stateCond_;    // State change condition
  int iState_;                  // Current state
};


#endif
