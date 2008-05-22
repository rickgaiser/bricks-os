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
        unwrapfunc1r(channelCreate,          pSndMsg, unsigned, iFlags);
        unwrapfunc1r(channelDestroy,         pSndMsg, int, iChannelID);
        unwrapfunc4r(channelConnectAttach,   pSndMsg, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
        unwrapfunc1r(channelConnectDetach,   pSndMsg, int, iConnectionID);
        // Names
        unwrapfunc2r(registerName,           pSndMsg, int, channelID, const char *, name);
        unwrapfunc3r(lookupName,             pSndMsg, const char *, name, int *, pid, int *, channelID);
        // Threads
        unwrapfunc4r(pthread_create,         pSndMsg, pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
        unwrapfunc1 (pthread_exit,           pSndMsg, void *, status);
        unwrapfunc2r(pthread_join,           pSndMsg, pthread_t, thread, void **, value_ptr);
        // Conditions
        unwrapfunc2r(pthread_cond_init,      pSndMsg, pthread_cond_t *, cond, const pthread_condattr_t *, attr);
        unwrapfunc1r(pthread_cond_destroy,   pSndMsg, pthread_cond_t *, cond);
        unwrapfunc2r(pthread_cond_wait,      pSndMsg, pthread_cond_t *, cond, pthread_mutex_t *, mutex);
        unwrapfunc3r(pthread_cond_timedwait, pSndMsg, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
        unwrapfunc1r(pthread_cond_signal,    pSndMsg, pthread_cond_t *, cond);
        unwrapfunc1r(pthread_cond_broadcast, pSndMsg, pthread_cond_t *, cond);
        // Mutexes
        unwrapfunc2r(pthread_mutex_init,     pSndMsg, pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
        unwrapfunc1r(pthread_mutex_destroy,  pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1r(pthread_mutex_lock,     pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1r(pthread_mutex_trylock,  pSndMsg, pthread_mutex_t *, mutex);
        unwrapfunc1r(pthread_mutex_unlock,   pSndMsg, pthread_mutex_t *, mutex);

        //unwrapfunc1r(brk,                    pSndMsg, void *, addr);
        //unwrapfunc1r(close,                  pSndMsg, int, iFD);
        //unwrapfunc1 (_exit,                  pSndMsg, int, iStatus);
        unwrapfunc0r(getpid);
        //unwrapfunc3r(read,                   pSndMsg, int, iFD, void *, pBuf, size_t, size);
        //unwrapfunc1r(sbrk,                   pSndMsg, intptr_t, increment);
        unwrapfunc1r(sleep,                  pSndMsg, unsigned int, iSeconds);
        unwrapfunc1r(usleep,                 pSndMsg, useconds_t, useconds);
        //unwrapfunc3r(write,                  pSndMsg, int, iFD, const void *, pBuf, size_t, size);

        default:
          printk("k_msgSend: Invalid function id: %d\n", pHeader->iFunctionID);
      };
    }
    else
    {
      printk("k_msgSend: Invalid kernel header(0x%x, %d, %d, 0x%x)\n", pHeader, pHeader->iHeaderSize, iSndSize, pHeader->iVersion);
    }
  }
  else if(iConnectionID < CONNECTION_ID_BASE)
  {
    // Kernel Drivers ID
    printk("k_msgSend: Invalid connection id: %d\n", iConnectionID);
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
