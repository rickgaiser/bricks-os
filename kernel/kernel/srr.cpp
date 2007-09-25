#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srrChannel_k.h"
#include "kernel/task.h"
#include "kernel/pthread_k.h"
#include "kernel/syscall.h"
#include "asm/cpu.h"
#include "asm/arch/config.h"
#include "string.h"


//---------------------------------------------------------------------------
int
k_msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

#ifndef CONFIG_DIRECT_ACCESS_KERNEL_FUNC
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
        unwrapfunc1r(channelCreate,          pSndMsg, unsigned, iFlags);
        unwrapfunc1r(channelDestroy,         pSndMsg, int, iChannelID);
        unwrapfunc4r(channelConnectAttach,   pSndMsg, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
        unwrapfunc1r(channelConnectDetach,   pSndMsg, int, iConnectionID);
        unwrapfunc4r(pthread_create,         pSndMsg, pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
        unwrapfunc1 (pthread_exit,           pSndMsg, void *, status);
        unwrapfunc2r(pthread_cond_init,      pSndMsg, pthread_cond_t *, cond, const pthread_condattr_t *, attr);
        unwrapfunc1r(pthread_cond_destroy,   pSndMsg, pthread_cond_t *, cond);
        unwrapfunc2r(pthread_cond_wait,      pSndMsg, pthread_cond_t *, cond, pthread_mutex_t *, mutex);
        unwrapfunc3r(pthread_cond_timedwait, pSndMsg, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
        unwrapfunc1r(pthread_cond_signal,    pSndMsg, pthread_cond_t *, cond);
        unwrapfunc1r(pthread_cond_broadcast, pSndMsg, pthread_cond_t *, cond);
        unwrapfunc2r(pthread_mutex_init,     pSndMsg, pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
        unwrapfunc1r(pthread_mutex_destroy,  pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1r(pthread_mutex_lock,     pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1r(pthread_mutex_trylock,  pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1r(pthread_mutex_unlock,   pSndMsg, pthread_mutex_t *, mutex);
        default:
          printk("k_msgSend: Invalid function id: %d\n", pHeader->iFunctionID);
      };
    }
    else
    {
      printk("k_msgSend: Invalid kernel header(0x%x, %d, %d, 0x%x)\n", pHeader, pHeader->iHeaderSize, iSndSize, pHeader->iVersion);
    }
  }
#else
  // Filter error ID
  if(iConnectionID <= 1)
  {
    printk("k_msgSend: Invalid connection id: %d\n", iConnectionID);
  }
#endif // CONFIG_DIRECT_ACCESS_KERNEL_FUNC
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
