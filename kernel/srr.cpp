#include "kernel/srr.h"
#include "kernel/srrKernel.h"
#include "kernel/task.h"
#include "iostream"


volatile bool bMsgSend(false);
volatile bool bMsgReply(false);
SMessage msg;


//---------------------------------------------------------------------------
int
k_channelCreate(SChannelCreate * args)
{
  int iRetVal(-1);

  // Locate empty channel in current task
  for(int iChannel(0); iChannel < MAX_CHANNEL_COUNT; iChannel++)
  {
    if(CTaskManager::pCurrentTask_->pChannel_[iChannel] == 0)
    {
      SChannel * chan = new SChannel;
      for(int iMsg(0); iMsg < MAX_MESSAGE_COUNT; iMsg++)
        chan->msg[iMsg].bUsed = false;
      CTaskManager::pCurrentTask_->pChannel_[iChannel] = chan;

      iRetVal = iChannel;

      break;
    }
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_channelDestroy(SChannelDestroy * args)
{
  int iRetVal(-1);
  int iChannelID(args->iChannelID);

  if((iChannelID >= 0) && (iChannelID < MAX_CHANNEL_COUNT))
  {
    if(CTaskManager::pCurrentTask_->pChannel_[iChannelID] != 0)
    {
      delete CTaskManager::pCurrentTask_->pChannel_[iChannelID];
      iRetVal = 0;
    }
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_connectAttach(SConnectAttach * args)
{
  int iRetVal(-1);

  if(args->iProcessID < MAX_TASK_COUNT)
  {
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_connectDetach(SConnectDetach * args)
{
  int iRetVal(-1);

  return iRetVal;
}

//---------------------------------------------------------------------------
// Kernel function call table
typedef int (*kernelFunction)(void * args);
const kernelFunction kfunctions[] =
{
    (kernelFunction)k_channelCreate    // 0
  , (kernelFunction)k_channelDestroy   // 1
  , (kernelFunction)k_connectAttach    // 2
  , (kernelFunction)k_connectDetach    // 3
};
const unsigned int kfunctions_size(sizeof(kfunctions) / sizeof(kernelFunction));

//---------------------------------------------------------------------------
int
k_msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

  // Filter error IDs
  if(iConnectionID <= 0)
  {
    std::cout<<"ERROR: Invalid iConnectionID: "<<iConnectionID<<std::endl;
  }
  else
  {
    switch(iConnectionID)
    {
      case scKERNEL:
      {
        SKernelMessageHeader * pHeader = (SKernelMessageHeader *)pSndMsg;

        if(pHeader->iFunctionID < kfunctions_size)
        {
          iRetVal = kfunctions[pHeader->iFunctionID]((void *)((unsigned char *)pSndMsg + sizeof(SKernelMessageHeader)));
        }
      }
      case scTEST:
      {
        // Create message
        msg.pSndMsg  = pSndMsg;
        msg.iSndSize = iSndSize;
        msg.pRcvMsg  = pRcvMsg;
        msg.iRcvSize = iRcvSize;
        // Send message
        bMsgSend = true;

        // Wait for reply message
        while(bMsgReply == false){}
        bMsgReply = false;

        // Return received byte count
        iRetVal = msg.iRetVal;;
        break;
      }
      default:
        ;
    };
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
    std::cout<<"ERROR: Invalid ChannelID: "<<iChannelID<<std::endl;
  }
  else
  {
    switch(iChannelID)
    {
      case scTEST:
      {
        // Wait for send message
        while(bMsgSend == false){}
        bMsgSend = false;

        // Copy send message
        int iSize = (iRcvSize > msg.iSndSize) ? msg.iSndSize : iRcvSize;
        for(int i(0); i < iSize; i++)
          ((char *)pRcvMsg)[i] = ((char *)msg.pSndMsg)[i];
        // Return received byte count
        iRetVal = iSize;
        break;
      }
      default:
      {
        /*
        if(iChannelID < 1)
          return -1;
        else
        {
          CProcess * pProcess = getProcess(iProcessID, true);
          CChannel * pChannel = 0;

          if(pProcess != 0)
          {
            // Locate channel
            for(unsigned int iChannel(0); iChannel < pProcess->channels_.size(); iChannel++)
            {
              if(pProcess->channels_[iChannel].iChannelID_ == iChannelID)
              {
                pChannel = &(pProcess->channels_[iChannel]);
                break;
              }
            }

            if(pChannel != 0)
            {
              // Wait for message to be sent
              pChannel->psem_.acquire();
              // Get sent message
              if(iRcvSize > pChannel->iDataSize_)
                iRcvSize = pChannel->iDataSize_;
              memcpy(pRcvMsg, pChannel->data_, iRcvSize);
              // Delete local copy
              delete pChannel->data_;

              // FIXME: Should be the receive id of the sender
              iRetVal = iChannelID;
            }
          }
        }
        */
      }
    };
  }

  return iRetVal;
}

//---------------------------------------------------------------------------
int
k_msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize)
{
  int iRetVal(-1);

  // Filter kernel and error IDs
  if(iReceiveID <= 0)
  {
    std::cout<<"ERROR: Invalid iReceiveID: "<<iReceiveID<<std::endl;
  }
  else
  {
    switch(iReceiveID)
    {
      case scTEST:
      {
        // Copy reply message
        int iSize = (iReplySize > msg.iRcvSize) ? msg.iRcvSize : iReplySize;
        for(int i(0); i < iSize; i++)
          ((char *)msg.pRcvMsg)[i] = ((char *)pReplyMsg)[i];
        // Return replied byte count (to sender)
        msg.iRetVal = iSize;
        // Return replied byte count (to replyer)
        iRetVal = iSize;

        bMsgReply = true;
        break;
      }
      default:
      {
        /*
        if(iReceiveID >= 1)
        {
          CProcess * pProcess = getProcess(iProcessID, true);
          CChannel * pChannel = 0;

          if(pProcess != 0)
          {
            // Locate channel
            for(unsigned int iChannel(0); iChannel < pProcess->channels_.size(); iChannel++)
            {
              if(pProcess->channels_[iChannel].iChannelID_ == iReceiveID)
              {
                pChannel = &(pProcess->channels_[iChannel]);
                break;
              }
            }

            if(pChannel != 0)
            {
              // Set reply data
              pChannel->rdata_ = new unsigned char[iReplySize];
              pChannel->iRDataSize_ = iReplySize;
              memcpy(pChannel->rdata_, pReplyMsg, iReplySize);

              // Notify sender
              pChannel->csem_.release();

              iRetVal = 0;
            }
          }
        }
        */
      }
    };
  }

  return iRetVal;
}
