#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srrChannel.h"
#include "kernel/srrConnection.h"
#include "kernel/task.h"
#include "kernel/pthread_k.h"
#include "kernel/unistd_k.h"
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
  if(iConnectionID <= 0)
  {
    // Invalid ID
    printk("k_msgSend: Invalid connection id: %d\n", iConnectionID);
  }
  else if(iConnectionID == scKERNEL)
  {
    // Kernel ID
    SKernelMessageHeader * pHeader = (SKernelMessageHeader *)pSndMsg;

    if((pHeader->iHeaderSize == sizeof(SKernelMessageHeader)) &&
       (pHeader->iVersion == INTERFACE_VERSION(1, 0)))
    {
      switch(pHeader->iFunctionID)
      {
        // Channels
        kunwrapfunc1r(channelCreate,          pSndMsg, unsigned, iFlags);
        kunwrapfunc1r(channelDestroy,         pSndMsg, int, iChannelID);
        kunwrapfunc4r(channelConnectAttach,   pSndMsg, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
        kunwrapfunc1r(channelConnectDetach,   pSndMsg, int, iConnectionID);
        // Names
        kunwrapfunc2r(registerName,           pSndMsg, int, channelID, const char *, name);
        kunwrapfunc3r(lookupName,             pSndMsg, const char *, name, int *, pid, int *, channelID);
        // Threads
        kunwrapfunc4r(pthread_create,         pSndMsg, pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
        kunwrapfunc1 (pthread_exit,           pSndMsg, void *, status);
        kunwrapfunc2r(pthread_join,           pSndMsg, pthread_t, thread, void **, value_ptr);
        // Conditions
        kunwrapfunc2r(pthread_cond_init,      pSndMsg, pthread_cond_t *, cond, const pthread_condattr_t *, attr);
        kunwrapfunc1r(pthread_cond_destroy,   pSndMsg, pthread_cond_t *, cond);
        kunwrapfunc2r(pthread_cond_wait,      pSndMsg, pthread_cond_t *, cond, pthread_mutex_t *, mutex);
        kunwrapfunc3r(pthread_cond_timedwait, pSndMsg, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
        kunwrapfunc1r(pthread_cond_signal,    pSndMsg, pthread_cond_t *, cond);
        kunwrapfunc1r(pthread_cond_broadcast, pSndMsg, pthread_cond_t *, cond);
        // Mutexes
        kunwrapfunc2r(pthread_mutex_init,     pSndMsg, pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
        kunwrapfunc1r(pthread_mutex_destroy,  pSndMsg, pthread_mutex_t *, mutex);
        kunwrapfunc1r(pthread_mutex_lock,     pSndMsg, pthread_mutex_t *, mutex);
        kunwrapfunc1r(pthread_mutex_trylock,  pSndMsg, pthread_mutex_t *, mutex);
        kunwrapfunc1r(pthread_mutex_unlock,   pSndMsg, pthread_mutex_t *, mutex);

        //kunwrapfunc1r(brk,                    pSndMsg, void *, addr);
        //kunwrapfunc1r(close,                  pSndMsg, int, iFD);
        //kunwrapfunc1 (_exit,                  pSndMsg, int, iStatus);
        kunwrapfunc0r(getpid);
        //kunwrapfunc3r(read,                   pSndMsg, int, iFD, void *, pBuf, size_t, size);
        //kunwrapfunc1r(sbrk,                   pSndMsg, intptr_t, increment);
        kunwrapfunc1r(sleep,                  pSndMsg, unsigned int, iSeconds);
        kunwrapfunc1r(usleep,                 pSndMsg, useconds_t, useconds);
        //kunwrapfunc3r(write,                  pSndMsg, int, iFD, const void *, pBuf, size_t, size);

        default:
          printk("k_msgSend: Invalid function id: %d\n", pHeader->iFunctionID);
      };
    }
    else
    {
      printk("k_msgSend: Invalid header(0x%x, %d, %d, 0x%x)\n", pHeader, pHeader->iHeaderSize, iSndSize, pHeader->iVersion);
    }
  }
  else if(iConnectionID < CONNECTION_ID_BASE)
  {
    // Kernel Drivers ID
    //if(kernelDriver[iConnectionID] != 0)
    //{
    //  iRetVal = kernelDriver[iConnectionID]->msgSend(iConnectionID, pSndMsg, iSndSize, pRcvMsg, iRcvSize);
    //}
    //else
    {
      printk("k_msgSend: Invalid connection id: %d\n", iConnectionID);
    }
  }
  else
#endif // CONFIG_DIRECT_ACCESS_KERNEL_FUNC
  {
    iRetVal = CTaskManager::pCurrentTask_->msgSend(iConnectionID, pSndMsg, iSndSize, pRcvMsg, iRcvSize);
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_msgReceive(int iChannelID, void * pRcvMsg, int iRcvSize)
{
  return CTaskManager::pCurrentTask_->msgReceive(iChannelID, pRcvMsg, iRcvSize);
}

//---------------------------------------------------------------------------
int
k_msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize)
{
  return CTaskManager::pCurrentTask_->msgReply(iReceiveID, iStatus, pReplyMsg, iReplySize);
}

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
