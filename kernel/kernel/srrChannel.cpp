#include "kernel/debug.h"
#include "kernel/srrChannel_k.h"
#include "kernel/task.h"
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
