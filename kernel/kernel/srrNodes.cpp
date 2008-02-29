#include "kernel/srrNodes.h"
#include "kernel/srr_k.h"
#include "kernel/debug.h"
#include "kernel/queue.h"
#include "kernel/task.h"
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
