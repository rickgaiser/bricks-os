#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srrChannel_k.h"
#include "kernel/task.h"
#include "string.h"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CChannel::CChannel()
 : pMsgWaiting_(NULL)
 , iState_(CHS_FREE)
{
  for(int i(0); i < MAX_IN_CONNECTION_COUNT; i++)
    pConnectionsIn_[i] = NULL;
}

//------------------------------------------------------------------------------
CChannel::~CChannel()
{
  // FIXME: Connections need to be disconnected!
}

//------------------------------------------------------------------------------
void
CChannel::msgSend(CConnection & connection)
{
  //printk("CChannel::msgSend\n");

  // Wait for channel to become free
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != CHS_FREE)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  // Set the waiting message, and notify msgReceive of the new message
  pMsgWaiting_ = &connection;
  iState_ = CHS_MSG_SENT;
  k_pthread_cond_broadcast(&stateCond_);
  k_pthread_mutex_unlock(&mutex_);
}

//------------------------------------------------------------------------------
int
CChannel::msgReceive(void * pRcvMsg, int iRcvSize)
{
  int iRetVal;

  //printk("CChannel::msgReceive\n");

  // Wait for a message
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != CHS_MSG_SENT)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  // Copy the data and receive id
  if(iRcvSize > pMsgWaiting_->iSndSize_)
    iRcvSize = pMsgWaiting_->iSndSize_;
  memcpy(pRcvMsg, pMsgWaiting_->pSndMsg_, iRcvSize);
  iRetVal = pMsgWaiting_->iReceiveID_;

  // Release the channel, so the next message can be sent
  iState_ = CHS_FREE;
  k_pthread_cond_broadcast(&stateCond_);
  k_pthread_mutex_unlock(&mutex_);

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CChannel::msgReply(int iConnectionIDX, int iStatus, const void * pReplyMsg, int iReplySize)
{
  int iRetVal(-1);

  // Validate connection
  if((iConnectionIDX >= 0) &&
     (iConnectionIDX < MAX_IN_CONNECTION_COUNT) &&
     (pConnectionsIn_[iConnectionIDX] != NULL))
  {
    // Reply the message
    iRetVal = pConnectionsIn_[iConnectionIDX]->msgReply(iStatus, pReplyMsg, iReplySize);
  }
  else
  {
    printk("CChannel::msgReply: Invalid connection id: %d\n", iConnectionIDX);
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CConnection::CConnection(CChannel * channel, int iReceiveID)
 : channel_(channel)
 , iReceiveID_(iReceiveID)
 , iState_(COS_FREE)
{
}

//------------------------------------------------------------------------------
CConnection::~CConnection()
{
  // FIXME: We need to be disconnected!
}

//------------------------------------------------------------------------------
int
CConnection::msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal;

  //printk("CConnection::msgSend\n");

  // Wait for connection to become free
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != COS_FREE)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  // Set message ready
  pSndMsg_  = pSndMsg;
  iSndSize_ = iSndSize;
  pRcvMsg_  = pRcvMsg;
  iRcvSize_ = iRcvSize;
  // Send message to channel
  channel_->msgSend(*this);

  // Wait for reply
  iState_ = COS_MSG_SENT;
  k_pthread_cond_broadcast(&stateCond_);
  while(iState_ != COS_MSG_REPLIED)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  iRetVal = iRetVal_;

  // Free the connection
  iState_ = COS_FREE;
  k_pthread_cond_broadcast(&stateCond_);
  k_pthread_mutex_unlock(&mutex_);

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CConnection::msgReply(int iStatus, const void * pReplyMsg, int iReplySize)
{
  int iRetVal(-1);

  //printk("CConnection::msgReply\n");

  // Lock the connection
  k_pthread_mutex_lock(&mutex_);

  if(iState_ == COS_MSG_SENT)
  {
    // Copy reply data and return status
    if(iReplySize < iRcvSize_)
      iReplySize = iRcvSize_;
    memcpy(pRcvMsg_, pReplyMsg, iReplySize);
    iRetVal_ = iStatus;

    iRetVal = 0;

    // Notify this->msgSend of replied message
    iState_ = COS_MSG_REPLIED;
    k_pthread_cond_broadcast(&stateCond_);
  }
  else
  {
    printk("CConnection::msgReply: Can't reply, no message sent\n");
  }

  // Unlock the connection
  k_pthread_mutex_unlock(&mutex_);

  return iRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int
k_channelCreate(unsigned iFlags)
{
  return CTaskManager::pCurrentTask_->channelCreate(iFlags);
}

//------------------------------------------------------------------------------
int
k_channelDestroy(int iChannelID)
{
  return CTaskManager::pCurrentTask_->channelDestroy(iChannelID);
}

//------------------------------------------------------------------------------
int
k_channelConnectAttach(uint32_t iNodeID, pid_t iProcessID, int iChannelID, int iFlags)
{
  return CTaskManager::pCurrentTask_->channelConnectAttach(iNodeID, iProcessID, iChannelID, iFlags);
}

//------------------------------------------------------------------------------
int
k_channelConnectDetach(int iConnectionID)
{
  return CTaskManager::pCurrentTask_->channelConnectDetach(iConnectionID);
}
