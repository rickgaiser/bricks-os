#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srrChannel_k.h"
#include "kernel/task.h"
#include "string.h"


enum EMessageType
{
  MT_SEND = 1,
  MT_REPLY = 2
};

struct SMessageHeader
{
  uint16_t type;   // Message type
  uint16_t length; // Total length of message, including header
  uint32_t id;
};

struct SSendMessage
{
  SMessageHeader header;
  int32_t iConnectionID;
  int32_t iRcvSize;
};

struct SReplyMessage
{
  SMessageHeader header;
  int32_t iStatus;
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CChannel::CChannel()
 : iState_(CS_FREE)
{
}

//------------------------------------------------------------------------------
CChannel::~CChannel()
{
}

//------------------------------------------------------------------------------
int
CChannel::msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal;

  // Wait for channel to become free
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != CS_FREE)
    k_pthread_cond_wait(&stateCond_, &mutex_);
  // Fill channel
  pSndMsg_  = pSndMsg;
  iSndSize_ = iSndSize;
  pRcvMsg_  = pRcvMsg;
  iRcvSize_ = iRcvSize;
  iState_   = CS_MSG_SEND;
  // Notify others of changed channel state
  k_pthread_cond_broadcast(&stateCond_);
  // Wait for message to be replied
  do
    k_pthread_cond_wait(&stateCond_, &mutex_);
  while(iState_ != CS_MSG_REPLIED);
  iRetVal = iRetVal_;
  iState_ = CS_FREE;
  // Notify others of changed channel state
  k_pthread_cond_broadcast(&stateCond_);
  // Unlock channel
  k_pthread_mutex_unlock(&mutex_);

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CChannel::msgReceive(int iChannelID, void * pRcvMsg, int iRcvSize)
{
  // Wait for message to be sent
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != CS_MSG_SEND)
    k_pthread_cond_wait(&stateCond_, &mutex_);
  if(iRcvSize > iSndSize_)
    iRcvSize = iSndSize_;
  memcpy(pRcvMsg, pSndMsg_, iRcvSize);
  iState_ = CS_MSG_RECEIVED;
  // Notify others of changed channel state
  k_pthread_cond_broadcast(&stateCond_);
  // Unlock channel
  k_pthread_mutex_unlock(&mutex_);

  return iChannelID;
}

//------------------------------------------------------------------------------
int
CChannel::msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize)
{
  int iRetVal(-1);

  // We can only reply a received message
  k_pthread_mutex_lock(&mutex_);
  if(iState_ == CS_MSG_RECEIVED)
  {
    // Copy reply data
    if(iReplySize < iRcvSize_)
      iReplySize = iRcvSize_;
    memcpy(pRcvMsg_, pReplyMsg, iReplySize);
    iRetVal_ = iStatus;
    iState_  = CS_MSG_REPLIED;
    // Notify others of changed channel state
    k_pthread_cond_broadcast(&stateCond_);
    // Unlock channel
    k_pthread_mutex_unlock(&mutex_);
    iRetVal = 0;
  }
  else
  {
    k_pthread_mutex_unlock(&mutex_);
    printk("k_msgReply: No message received, can't reply\n");
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CRemoteConnection::CRemoteConnection(IFileIO & tunnel)
 : tunnel_(tunnel)
 , iMsgID_(1)
{
//  tunnel_.setWriteBack(*this);
}

//------------------------------------------------------------------------------
CRemoteConnection::~CRemoteConnection()
{
//  tunnel_.removeWriteBack(*this);
}

//------------------------------------------------------------------------------
int
CRemoteConnection::msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  SSendMessage msg;

  msg.header.type   = MT_SEND;
  msg.header.length = sizeof(SSendMessage) + iSndSize;
  msg.header.id     = iMsgID_++;
  msg.iConnectionID = iConnectionID;
  msg.iRcvSize      = iRcvSize;

  // Wait for tunnel to become free
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != CS_FREE)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  // Set data ready for callback
  pRcvMsg_  = (uint8_t *)pRcvMsg;
  iRcvSize_ = iRcvSize;
  // Send data
  tunnel_.write(&msg, sizeof(SSendMessage));
  if(iSndSize > 0)
    tunnel_.write(pSndMsg, iSndSize);

  // Wait for message to be replied
  do
    k_pthread_cond_wait(&stateCond_, &mutex_);
  while(iState_ != CS_MSG_REPLIED);
  iState_ = CS_FREE;
  // Notify others of changed channel state
  k_pthread_cond_broadcast(&stateCond_);
  // Unlock tunnel
  k_pthread_mutex_unlock(&mutex_);

  return iStatus_;
}

// -----------------------------------------------------------------------------
int
CRemoteConnection::msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize)
{
  SReplyMessage msg;

  msg.header.type   = MT_REPLY;
  msg.header.length = sizeof(SReplyMessage) + iReplySize;
  msg.header.id     = iReceiveID;
  msg.iStatus       = iStatus;

  // Lock tunnel
  k_pthread_mutex_lock(&mutex_);

  // Send data
  tunnel_.write(&msg, sizeof(SReplyMessage));
  if(iReplySize > 0)
    tunnel_.write(pReplyMsg, iReplySize);

  // Unlock tunnel
  k_pthread_mutex_unlock(&mutex_);

  return 0;
}

// -----------------------------------------------------------------------------
// Write-back handler for channel
ssize_t
CRemoteConnection::write(const void * buffer, size_t size, loff_t *)
{
  for(size_t i(0); i < size; i++)
  {
    if(iBufferIndex_ < BUFFER_SIZE)
      ((uint8_t *)buffer_)[iBufferIndex_++] = ((uint8_t *)buffer)[i];
    else
    {
      printk("CSRRTunnel::write: ERROR: buffer overflow\n");
      iBufferIndex_ = 0;
      return 0;
    }
  }

  if(iBufferIndex_ >= sizeof(SMessageHeader))
  {
    SMessageHeader * header = (SMessageHeader *)buffer_;
    if(iBufferIndex_ >= header->length)
    {
      if(iBufferIndex_ > header->length)
        printk("CSRRTunnel::write: Warning: bytes left at end of message\n");

      switch(header->type)
      {
        // Remote node sends us a message
        case MT_SEND:
        {
          SSendMessage * msg = (SSendMessage *)buffer_;
          int status;

          printk("CSRRTunnel::write: Message received: send\n");

          if(msg->iRcvSize > 0)
          {
            uint8_t * rMsg = new uint8_t[msg->iRcvSize];

            // Send the message to the local connection
            status = k_msgSend(msg->iConnectionID, (uint8_t *)buffer_ + sizeof(SSendMessage), header->length - sizeof(SSendMessage), rMsg, msg->iRcvSize);

            // Return the reply to the remote node
            this->msgReply(msg->header.id, status, rMsg, msg->iRcvSize);
          }
          else
          {
            // Send the message to the local connection
            status = k_msgSend(msg->iConnectionID, (uint8_t *)buffer_ + sizeof(SSendMessage), header->length - sizeof(SSendMessage), 0, 0);

            // Return the reply to the remote node
            this->msgReply(msg->header.id, status, 0, 0);
          }

          break;
        }
        // Remote node replies a message
        case MT_REPLY:
        {
          SReplyMessage * msg = (SReplyMessage *)buffer_;

          printk("CSRRTunnel::write: Message received: reply\n");

          // Reply size
          iRcvSize_ = (iRcvSize_ < (msg->header.length - sizeof(SReplyMessage))) ? iRcvSize_ : (msg->header.length - sizeof(SReplyMessage));
          // Reply data
          if(iRcvSize_ > 0)
            memcpy(pRcvMsg_, (uint8_t *)buffer_ + sizeof(SReplyMessage), iRcvSize_);
          // Reply status
          iStatus_  = msg->iStatus;

          break;
        }
        default:
          printk("CSRRTunnel::write: ERROR: unknown message received\n");
      };

      iBufferIndex_ = 0;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int
k_channelCreate(unsigned iFlags)
{
  int iRetVal(-1);

  //printk("k_channelCreate\n");

  // Locate empty channel in current task
  for(int iChannel(0); iChannel < MAX_CHANNEL_COUNT; iChannel++)
  {
    if(CTaskManager::pCurrentTask_->pChannel_[iChannel] == NULL)
    {
      CTaskManager::pCurrentTask_->pChannel_[iChannel] = new CChannel;
      iRetVal = CHANNEL_IDX_TO_ID(iChannel);
      break;
    }
  }

  if(iRetVal == -1)
  {
    printk("k_channelCreate: Max channels reached!\n");
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
k_channelDestroy(int iChannelID)
{
  int iRetVal(-1);

  iChannelID = CHANNEL_ID_TO_IDX(iChannelID);

  if((iChannelID >= 0) &&
     (iChannelID < MAX_CHANNEL_COUNT) &&
     (CTaskManager::pCurrentTask_->pChannel_[iChannelID] != NULL))
  {
    // Channel valid, remove it
    delete CTaskManager::pCurrentTask_->pChannel_[iChannelID];
    CTaskManager::pCurrentTask_->pChannel_[iChannelID] = NULL;
    iRetVal = 0;
  }
  else
  {
    printk("k_channelDestroy: Invalid channel id: %d\n", CHANNEL_IDX_TO_ID(iChannelID));
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
k_channelConnectAttach(uint32_t iNodeID, pid_t iProcessID, int iChannelID, int iFlags)
{
  int iRetVal(-1);
  CTask * pTask;

  //printk("k_channelConnectAttach\n");

  iChannelID = CHANNEL_ID_TO_IDX(iChannelID);

  if(iNodeID == 0)
  {
    pTask = CTaskManager::getTaskFromPID(iProcessID);
    if(pTask != NULL)
    {
      // Process found, find channel
      if((iChannelID >= 0) &&
         (iChannelID < MAX_CHANNEL_COUNT) &&
         (pTask->pChannel_[iChannelID] != NULL))
      {
        // Channel found, find empty connection
        for(int i(0); i < MAX_CONNECTION_COUNT; i++)
        {
          if(CTaskManager::pCurrentTask_->pConnection_[i] == NULL)
          {
            CTaskManager::pCurrentTask_->pConnection_[i] = pTask->pChannel_[iChannelID];
            iRetVal = CONNECTION_IDX_TO_ID(i);
            break;
          }
        }
        if(iRetVal == -1)
        {
          printk("k_connectAttach: Max connections reached!\n");
        }
      }
      else
      {
        printk("k_connectAttach: Channel id not found: %d\n", CHANNEL_IDX_TO_ID(iChannelID));
      }
    }
    else
    {
      printk("k_connectAttach: Process id not found: %d\n", iProcessID);
    }
  }
  else
  {
    printk("k_connectAttach: Remote nodes not supported\n");
    //iRetVal = node[iNodeID].connectAttach(iProcessID, iChannelID, iFlags);
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
k_channelConnectDetach(int iConnectionID)
{
  int iRetVal(-1);
  iConnectionID = CONNECTION_ID_TO_IDX(iConnectionID);

  if((iConnectionID >= 0) &&
     (iConnectionID < MAX_CONNECTION_COUNT) &&
     (CTaskManager::pCurrentTask_->pConnection_[iConnectionID] != NULL))
  {
    // Connection valid, remove it
    CTaskManager::pCurrentTask_->pConnection_[iConnectionID] = NULL;
    iRetVal = 0;
  }
  else
  {
    printk("k_channelConnectDetach: Connection id not found: %d\n", CONNECTION_IDX_TO_ID(iConnectionID));
  }

  return iRetVal;
}
