#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srr_s.h"
#include "kernel/srrChannel_k.h"
#include "kernel/srrChannel_s.h"
#include "kernel/task.h"
#include "string.h"


//---------------------------------------------------------------------------
int
k_channelCreate(SChannelCreate * args)
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
k_channelDestroy(SChannelDestroy * args)
{
  int iRetVal(-1);
  int iChannelID(args->iChannelID - 2);

  if((iChannelID >= 0) &&
     (iChannelID < MAX_CHANNEL_COUNT) &&
     (CTaskManager::pCurrentTask_->pChannel_[iChannelID] != NULL))
  {
    // Channel valid, remove it
    delete CTaskManager::pCurrentTask_->pChannel_[iChannelID];
    CTaskManager::pCurrentTask_->pChannel_[iChannelID] = NULL;
    iRetVal = 0;
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_channelConnectAttach(SConnectAttach * args)
{
  int iRetVal(-1);
  int iChannelID(args->iChannelID - 2);
  CTask * pTask;

  printk("k_channelConnectAttach\n");

  if(args->iNodeID == 0)
  {
    TAILQ_FOREACH(pTask, &CTaskManager::task_queue, task_qe)
    {
      if(pTask->iPID_ == args->iProcessID)
      {
        // Process found, find channel
        if((iChannelID < MAX_CHANNEL_COUNT) && (pTask->pChannel_[iChannelID] != NULL))
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
          printk("k_connectAttach: Channel not found\n");
        }
        break;
      }
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
k_channelConnectDetach(SConnectDetach * args)
{
  int iRetVal(-1);
  int iConnectionID(args->iConnectionID - 2);
  
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
    printk("k_channelConnectDetach: Invalid connection id: %d\n", args->iConnectionID);
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
    if(pHeader->iHeaderSize != sizeof(SKernelMessageHeader))
      panic("k_msgSend: Invalid header size");
    if(pHeader->iVersion != INTERFACE_VERSION(1, 0))
      panic("k_msgSend: Invalid header version");

    switch(pHeader->iFunctionID)
    {
      case kfCHANNEL_CREATE:
        iRetVal = k_channelCreate       ((SChannelCreate *) ((unsigned char *)pSndMsg + sizeof(SKernelMessageHeader)));
        break;
      case kfCHANNEL_DESTROY:
        iRetVal = k_channelDestroy      ((SChannelDestroy *)((unsigned char *)pSndMsg + sizeof(SKernelMessageHeader)));
        break;
      case kfCHANNEL_ATTACH:
        iRetVal = k_channelConnectAttach((SConnectAttach *) ((unsigned char *)pSndMsg + sizeof(SKernelMessageHeader)));
        break;
      case kfCHANNEL_DETACH:
        iRetVal = k_channelConnectDetach((SConnectDetach *) ((unsigned char *)pSndMsg + sizeof(SKernelMessageHeader)));
        break;
      default:
        printk("k_msgSend: Invalid function id: %d\n", pHeader->iFunctionID);
    };
  }
  else
  {
    iConnectionID -= 2;
    if((iConnectionID < MAX_CONNECTION_COUNT) && (CTaskManager::pCurrentTask_->pConnection_[iConnectionID] != NULL))
    {
      SChannel * pChannel = CTaskManager::pCurrentTask_->pConnection_[iConnectionID];
      // Wait for channel to become free
      // FIXME: Busy waiting
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
      printk("k_msgSend: Connection not found\n");
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
    if((iChannelID < MAX_CHANNEL_COUNT) && (CTaskManager::pCurrentTask_->pChannel_[iChannelID] != NULL))
    {
      SChannel * pChannel = CTaskManager::pCurrentTask_->pChannel_[iChannelID];
      // Wait for message to be sent
      // FIXME: Busy waiting
      while(pChannel->iState != CS_MSG_SEND);
      if(pChannel->iSndSize < iRcvSize)
        iRcvSize = pChannel->iSndSize;
      memcpy(pRcvMsg, pChannel->pSndMsg, iRcvSize);
      pChannel->iState = CS_MSG_RECEIVED;
      iRetVal  = iChannelID + 2;
    }
    else
    {
      printk("k_msgReceive: Channel not found\n");
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
    if((iReceiveID < MAX_CHANNEL_COUNT) && (CTaskManager::pCurrentTask_->pChannel_[iReceiveID] != NULL))
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
    }
    else
    {
      printk("k_msgReply: Receive id not found\n");
    }
  }

  return iRetVal;
}
