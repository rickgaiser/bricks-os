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


#include "kernel/srrNodes.h"
#include "kernel/srr_k.h"
#include "kernel/debug.h"
#include "kernel/queue.h"
#include "kernel/task.h"
#include "kernel/taskManager.h"
#include "string.h"


CGateway * nodes_[MAX_NODES];


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGateway::CGateway(int iPID, int iChannelIDControl, int iChannelIDData)
{
}

//---------------------------------------------------------------------------
CGateway::~CGateway()
{
}

//---------------------------------------------------------------------------
int
CGateway::channelConnectAttach(pid_t iProcessID, int iChannelID, int iFlags)
{
  return -1;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int
k_registerNodeGateway(int channelIDControl, int channelIDData)
{
  int iRetVal(-1);

  //printk("k_registerNodeGateway(%d,%s)\n", channelIDControl, channelIDData);

  for(int iNodeIndex(0); iNodeIndex < MAX_NODES; iNodeIndex++)
  {
    if(nodes_[iNodeIndex] == NULL)
    {
      nodes_[iNodeIndex] = new CGateway(CTaskManager::pCurrentTask_->iPID_, channelIDControl, channelIDData);

      iRetVal = NODEIDX_TO_NODEID(iNodeIndex);

      break;
    }
  }

  return iRetVal;
}
