#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srrChannel_k.h"
#include "kernel/srrConnection_k.h"
#include "kernel/task.h"
#include "string.h"


//------------------------------------------------------------------------------
CConnection::CConnection()
 : channel_(NULL)
 , iReceiveID_(-1)
 , iState_(COS_FREE)
{
}

//------------------------------------------------------------------------------
CConnection::~CConnection()
{
  this->disconnect();
}

//------------------------------------------------------------------------------
bool
CConnection::connect(CChannel * channel)
{
  bool bRetVal(false);

  // Validate parameter
  if(channel != NULL)
  {
    // Disconnect first if we're already connected
    if(channel_ != NULL)
      this->disconnect();

    iReceiveID_ = channel_->addConnection(this);
    if(iReceiveID_ >= 0)
    {
      channel_ = channel;
      bRetVal = true;
    }
  }

  return bRetVal;
}

//------------------------------------------------------------------------------
void
CConnection::disconnect()
{
  // Wait for connection to become free
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != COS_FREE)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  if(channel_ != NULL)
  {
    channel_->removeConnection(this);

    channel_    = NULL;
    iReceiveID_ = -1;
  }

  // Free the connection
  k_pthread_mutex_unlock(&mutex_);
}

//------------------------------------------------------------------------------
void
CConnection::setReceiveID(int iReceiveID)
{
  iReceiveID_ = iReceiveID;
}

//------------------------------------------------------------------------------
int
CConnection::getReceiveID()
{
  return iReceiveID_;
}

//------------------------------------------------------------------------------
int
CConnection::msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

  //printk("CConnection::msgSend\n");

  if(channel_ != NULL)
  {
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
  }
  else
  {
    printk("CConnection::msgSend: ERROR: Not connected\n");
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CConnection::msgReply(int iStatus, const void * pReplyMsg, int iReplySize)
{
  int iRetVal(-1);

  //printk("CConnection::msgReply\n");

  if(channel_ != NULL)
  {
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
  }
  else
  {
    printk("CConnection::msgReply: ERROR: Not connected\n");
  }

  return iRetVal;
}
