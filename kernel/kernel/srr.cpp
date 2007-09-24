#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srr_s.h"
#include "kernel/srrChannel_k.h"
#include "kernel/srrChannel_s.h"
#include "kernel/task.h"
#include "kernel/pthread_k.h"
#include "kernel/syscall.h"
#include "asm/cpu.h"
#include "string.h"


//---------------------------------------------------------------------------
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
      SChannel * chan = new SChannel;
      chan->iState = CS_FREE;
      CTaskManager::pCurrentTask_->pChannel_[iChannel] = chan;

      iRetVal = iChannel + 2; // Channels 0&1 are reserved for error&kernel

      break;
    }
  }

  if(iRetVal == -1)
  {
    printk("k_channelCreate: Max channels reached!\n");
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_channelDestroy(int iChannelID)
{
  int iRetVal(-1);

  iChannelID -= 2;

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
    printk("k_channelDestroy: Invalid channel id: %d\n", iChannelID + 2);
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_channelConnectAttach(uint32_t iNodeID, pid_t iProcessID, int iChannelID, int iFlags)
{
  int iRetVal(-1);
  CTask * pTask;

  //printk("k_channelConnectAttach\n");

  iChannelID -= 2;

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
            iRetVal = i + 2;
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
        printk("k_connectAttach: Channel id not found: %d\n", iChannelID + 2);
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
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_channelConnectDetach(int iConnectionID)
{
  int iRetVal(-1);
  iConnectionID -= 2;

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
    printk("k_channelConnectDetach: Connection id not found: %d\n", iConnectionID + 2);
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

  // Filter error ID
  if(iConnectionID <= 0)
  {
    printk("k_msgSend: Invalid connection id: %d\n", iConnectionID);
  }
  else if(iConnectionID == scKERNEL)
  {
    SKernelMessageHeader * pHeader = (SKernelMessageHeader *)pSndMsg;

    if((pHeader->iHeaderSize == sizeof(SKernelMessageHeader)) &&
       (pHeader->iVersion == INTERFACE_VERSION(1, 0)))
    {
      switch(pHeader->iFunctionID)
      {
        unwrapfunc1(channelCreate,          pSndMsg, unsigned, iFlags);
        unwrapfunc1(channelDestroy,         pSndMsg, int, iChannelID);
        unwrapfunc4(channelConnectAttach,   pSndMsg, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
        unwrapfunc1(channelConnectDetach,   pSndMsg, int, iConnectionID);
        unwrapfunc4(pthread_create,         pSndMsg, pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
//        unwrapfunc1(pthread_exit,           pSndMsg, void *, status);
        unwrapfunc2(pthread_cond_init,      pSndMsg, pthread_cond_t *, cond, const pthread_condattr_t *, attr);
        unwrapfunc1(pthread_cond_destroy,   pSndMsg, pthread_cond_t *, cond);
        unwrapfunc2(pthread_cond_wait,      pSndMsg, pthread_cond_t *, cond, pthread_mutex_t *, mutex);
        unwrapfunc3(pthread_cond_timedwait, pSndMsg, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
        unwrapfunc1(pthread_cond_signal,    pSndMsg, pthread_cond_t *, cond);
        unwrapfunc1(pthread_cond_broadcast, pSndMsg, pthread_cond_t *, cond);
        unwrapfunc2(pthread_mutex_init,     pSndMsg, pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
        unwrapfunc1(pthread_mutex_destroy,  pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1(pthread_mutex_lock,     pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1(pthread_mutex_trylock,  pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1(pthread_mutex_unlock,   pSndMsg, pthread_mutex_t *, mutex);
        default:
          printk("k_msgSend: Invalid function id: %d\n", pHeader->iFunctionID);
      };
    }
    else
    {
      printk("k_msgSend: Invalid kernel header(0x%x, %d, %d, 0x%x)\n", pHeader, pHeader->iHeaderSize, iSndSize, pHeader->iVersion);
    }
  }
  else
  {
    iConnectionID -= 2;
    if((iConnectionID >= 0) &&
       (iConnectionID < MAX_CONNECTION_COUNT) &&
       (CTaskManager::pCurrentTask_->pConnection_[iConnectionID] != NULL))
    {
      SChannel * pChannel = CTaskManager::pCurrentTask_->pConnection_[iConnectionID];
      // Wait for channel to become free
      // FIXME: Busy waiting, need to enable interrupt to prevent crash
      local_irq_enable();
      while(pChannel->iState != CS_FREE);
      pChannel->iState   = CS_USED;
      pChannel->pSndMsg  = pSndMsg;
      pChannel->iSndSize = iSndSize;
      pChannel->pRcvMsg  = pRcvMsg;
      pChannel->iRcvSize = iRcvSize;
      pChannel->iState   = CS_MSG_SEND;
      // Wait for message to be replied
      // FIXME: Busy waiting
      while(pChannel->iState != CS_MSG_REPLIED);
      iRetVal = pChannel->iRetVal;
      pChannel->iState   = CS_FREE;
    }
    else
    {
      printk("k_msgSend: Invalid connection id: %d\n", iConnectionID + 2);
    }
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_msgReceive(int iChannelID, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

  // Filter kernel and error IDs
  if(iChannelID <= 1)
  {
    printk("k_msgReceive: Invalid channel id: %d\n", iChannelID);
  }
  else
  {
    iChannelID -= 2;
    if((iChannelID >= 0) &&
       (iChannelID < MAX_CHANNEL_COUNT) &&
       (CTaskManager::pCurrentTask_->pChannel_[iChannelID] != NULL))
    {
      SChannel * pChannel = CTaskManager::pCurrentTask_->pChannel_[iChannelID];
      // Wait for message to be sent
      // FIXME: Busy waiting, need to enable interrupt to prevent crash
      local_irq_enable();
      while(pChannel->iState != CS_MSG_SEND);
      if(pChannel->iSndSize < iRcvSize)
        iRcvSize = pChannel->iSndSize;
      memcpy(pRcvMsg, pChannel->pSndMsg, iRcvSize);
      pChannel->iState = CS_MSG_RECEIVED;
      iRetVal  = iChannelID + 2;
    }
    else
    {
      printk("k_msgReceive: Invalid channel id: %d\n", iChannelID + 2);
    }
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize)
{
  int iRetVal(-1);

  // Filter kernel and error IDs
  if(iReceiveID <= 1)
  {
    printk("ERROR: Invalid receive id: %d\n", iReceiveID);
  }
  else
  {
    iReceiveID -= 2;
    if((iReceiveID >= 0) &&
       (iReceiveID < MAX_CHANNEL_COUNT) &&
       (CTaskManager::pCurrentTask_->pChannel_[iReceiveID] != NULL))
    {
      SChannel * pChannel = CTaskManager::pCurrentTask_->pChannel_[iReceiveID];
      // We can only reply a received message
      if(pChannel->iState == CS_MSG_RECEIVED)
      {
        // Copy reply data
        if(iReplySize < pChannel->iRcvSize)
          iReplySize = pChannel->iRcvSize;
        memcpy(pChannel->pRcvMsg, pReplyMsg, iReplySize);
        pChannel->iRetVal = iStatus;
        pChannel->iState = CS_MSG_REPLIED;
        iRetVal = 0;
      }
      else
      {
        printk("k_msgReply: No message received, can't reply\n");
      }
    }
    else
    {
      printk("k_msgReply: Invalid receive id: %d\n", iReceiveID + 2);
    }
  }

  return iRetVal;
}
