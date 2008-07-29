#include "kernel/fileDriver.h"
#include "kernel/debug.h"
#include "kernel/task.h"
#include "kernel/unistd_k.h"
#include "kernel/srr_k.h"
#include "stddef.h"
#include "fcntl.h"


// -----------------------------------------------------------------------------
CKernelFileDriver::CKernelFileDriver(IFileIO & file, const char * name)
 : file_(file)
{
  int iChannelID = CTaskManager::pCurrentTask_->channelRegister(this, 0);
  if(iChannelID >= 0)
    k_registerName(iChannelID, name);

  for(int i(0); i < MAX_OPEN_FILES; i++)
    flags_[i] = 0;
}

// -----------------------------------------------------------------------------
CKernelFileDriver::~CKernelFileDriver()
{
}

// -----------------------------------------------------------------------------
int
CKernelFileDriver::msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  SFileHeader * pHeader = (SFileHeader *)pSndMsg;
  int iReceiveID(0);
  int iRetVal(-1);

  if(iReceiveID < MAX_OPEN_FILES)
  {
    switch(pHeader->command)
    {
      case FC_OPEN:
        flags_[iReceiveID] = ((SFileOpenHeader *)pHeader)->flags;
        break;
      case FC_CLOSE:
        flags_[iReceiveID] = 0;
        break;
      case FC_READ:
      {
        if(!(flags_[iReceiveID] & O_WRONLY))
        {
          int iSize = ((SFileReadHeader *)pHeader)->size;
          if(iSize > iRcvSize)
            iSize = iRcvSize;

          iRetVal = file_.read(pRcvMsg, iSize, !(flags_[iReceiveID] & O_NONBLOCK));
        }
        else
          printk("CKernelFileDriver: Cannot read from O_WRONLY file\n");
        break;
      }
      case FC_WRITE:
      {
        if(!(flags_[iReceiveID] & O_RDONLY))
        {
          iRetVal = file_.write((void *)((uint8_t *)pSndMsg + sizeof(SFileWriteHeader)), ((SFileWriteHeader *)pHeader)->size, !(flags_[iReceiveID] & O_NONBLOCK));
        }
        else
          printk("CKernelFileDriver: Cannot write to O_RDONLY file\n");
        break;
      }
      default:
        printk("CKernelFileDriver: Unknown command %d\n", pHeader->command);
    };
  }
  else
    printk("CKernelFileDriver: receive id too high %d\n", iReceiveID);

  return iRetVal;
}
