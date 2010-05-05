/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "kernel/fileDriver.h"
#include "kernel/debug.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"
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
