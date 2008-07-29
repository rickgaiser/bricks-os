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
class IConnection;
class CConnectionUser;
class CConnectionKernel;

//------------------------------------------------------------------------------
class IChannel
{
public:
  virtual ~IChannel(){}

  virtual bool createConnection(IConnection ** connection) = 0;
  virtual void removeConnection(IConnection  * connection) = 0;

  virtual int  msgReceive(void * pRcvMsg, int iRcvSize) = 0;
};

//------------------------------------------------------------------------------
class CAChannel
 : public IChannel
{
public:
  virtual ~CAChannel();

  virtual bool createConnection(IConnection ** connection) = 0;
  void removeConnection(IConnection * connection);

  virtual int  msgReceive(void * pRcvMsg, int iRcvSize) = 0;

protected:
  CAChannel();
  int  addConnection(IConnection * connection);

protected:
  // Connections to this channels (CTask has ownership!)
  // NOTE: We are using an array here becouse it is FAST, and we need the
  //       msgReply function to locate connections FAST.
  IConnection * pConnectionsIn_[MAX_IN_CONNECTION_COUNT];
};

//------------------------------------------------------------------------------
class CChannelUser
 : public CAChannel
{
public:
  CChannelUser();
  ~CChannelUser();

  bool createConnection(IConnection ** connection);

  void msgSend(CConnectionUser & connection);
  int  msgReceive(void * pRcvMsg, int iRcvSize);

private:
  CConnectionUser * pMsgWaiting_; // Connection that has a message waiting

  pthread_mutex_t mutex_;       // The channel lock
  pthread_cond_t stateCond_;    // State change condition
  int iState_;                  // Current state
};

//------------------------------------------------------------------------------
class CAChannelKernel
 : public CAChannel
{
public:
  CAChannelKernel();
  virtual ~CAChannelKernel();

  bool createConnection(IConnection ** connection);

  virtual int msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize) = 0;
  int msgReceive(void * pRcvMsg, int iRcvSize);
};


#endif
