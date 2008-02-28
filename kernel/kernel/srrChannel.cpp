#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srrChannel_k.h"
#include "kernel/srrConnection_k.h"
#include "kernel/task.h"
#include "string.h"


//------------------------------------------------------------------------------
CChannel::CChannel()
 : pMsgWaiting_(NULL)
 , iState_(CHS_FREE)
{
  memset(pConnectionsIn_, 0, sizeof(pConnectionsIn_));
}

//------------------------------------------------------------------------------
CChannel::~CChannel()
{
  // FIXME: Connections need to be disconnected!
}

//------------------------------------------------------------------------------
int
CChannel::addConnection(CConnection * connection)
{
  int iRetVal(-1);

  //printk("CChannel::addConnection\n");

  // Find empty connection in channel
  for(int iCOIDX(0); iCOIDX < MAX_IN_CONNECTION_COUNT; iCOIDX++)
  {
    //printk("CChannel::addConnection: 0x%x\n", pConnectionsIn_[iCOIDX]);
    if(pConnectionsIn_[iCOIDX] == NULL)
    {
      // Add new connection
      pConnectionsIn_[iCOIDX] = connection;

      // Return connection index
      iRetVal = iCOIDX;

      break;
    }
  }

  if(iRetVal < 0)
    printk("CChannel::addConnection: ERROR: Max connections reached\n");

  return iRetVal;
}

//------------------------------------------------------------------------------
void
CChannel::removeConnection(CConnection * connection)
{
  for(int iCOIDX(0); iCOIDX < MAX_IN_CONNECTION_COUNT; iCOIDX++)
    if(pConnectionsIn_[iCOIDX] == connection)
      pConnectionsIn_[iCOIDX] = NULL;
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
