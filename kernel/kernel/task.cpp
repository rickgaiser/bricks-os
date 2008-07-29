#include "kernel/debug.h"
#include "kernel/task.h"
#include "kernel/genwait.h"
#include "kernel/srr_k.h"
#include "kernel/srrChannel.h"
#include "kernel/srrConnection.h"
#include "kernel/srrNodes.h"
#include "asm/cpu.h"
#include "asm/task.h"
#include "string.h"


CTask        * CTaskManager::pCurrentTask_ = NULL;
CThread      * CTaskManager::pCurrentThread_ = NULL;
CThread      * CTaskManager::pIdleThread_  = NULL;
STaskQueue     CTaskManager::task_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::task_queue);
SThreadQueue   CTaskManager::thread_queue  = TAILQ_HEAD_INITIALIZER(CTaskManager::thread_queue);
SThreadQueue   CTaskManager::ready_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::ready_queue);
SThreadQueue   CTaskManager::timer_queue   = TAILQ_HEAD_INITIALIZER(CTaskManager::timer_queue);
SThreadQueue   CTaskManager::wait_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::wait_queue);
SThreadQueue   CTaskManager::dead_queue    = TAILQ_HEAD_INITIALIZER(CTaskManager::dead_queue);
uint32_t       CTaskManager::iPIDCount_    = 1;
useconds_t     CTaskManager::iCurrentTime_ = 0;


// -----------------------------------------------------------------------------
CThread::CThread(CTask * task)
 : iTimeout_(0)
 , pWaitObj_(NULL)
 , iWaitReturn_(0)
 , pTask_(task)
 , pParent_(NULL)
#ifdef CONFIG_MMU
 , impl_(&task->aspace())
#else
 , impl_()
#endif
 , eState_(TS_UNINITIALIZED)
{
  TAILQ_INIT(&children_queue);

  // Insert in the "all threads" queue
  TAILQ_INSERT_TAIL(&CTaskManager::thread_queue, this, thread_qe);
}

// -----------------------------------------------------------------------------
CThread::~CThread()
{
  // Remove from the "all threads" queue
  TAILQ_REMOVE(&CTaskManager::thread_queue, this, thread_qe);
}

// -----------------------------------------------------------------------------
void
CThread::state(EThreadState state)
{
  // State change?
  if(eState_ != state)
  {
    // Remove from current state queue
    switch(eState_)
    {
    case TS_READY:
      TAILQ_REMOVE(&CTaskManager::ready_queue, this, state_qe);
      break;
    case TS_RUNNING:
      CTaskManager::pCurrentTask_   = NULL;
      CTaskManager::pCurrentThread_ = NULL;
      break;
    case TS_WAITING:
      // Remove from timer queue
      if(this->iTimeout_ > 0)
      {
        this->iTimeout_ = 0;
        TAILQ_REMOVE(&CTaskManager::timer_queue, this, state_qe);
      }
      // Remove from wait queue
      if(this->pWaitObj_ != NULL)
      {
        this->pWaitObj_ = NULL;
        TAILQ_REMOVE(&CTaskManager::wait_queue, this, wait_qe);
      }
      break;
    case TS_DEAD:
      TAILQ_REMOVE(&CTaskManager::dead_queue, this, state_qe);
      break;
    default:
      ;
    };

    eState_ = state;

    // Add to new state queue
    switch(eState_)
    {
    case TS_READY:
      if(this != CTaskManager::pIdleThread_)
      {
        TAILQ_INSERT_TAIL(&CTaskManager::ready_queue, this, state_qe);
      }
      break;
    case TS_RUNNING:
      if(CTaskManager::pCurrentThread_ != NULL)
        CTaskManager::pCurrentThread_->state(TS_READY);
      CTaskManager::pCurrentTask_   = this->pTask_;
      CTaskManager::pCurrentThread_ = this;
      break;
    case TS_WAITING:
      {
        CThread * pThread;

        if((this->iTimeout_ <= 0) && (this->pWaitObj_ == NULL))
          panic("CThread::state: What are we waiting for???\n");

        // Place task in sorted timer queue, if timeout present
        if(this->iTimeout_ > 0)
        {
          TAILQ_FOREACH(pThread, &CTaskManager::timer_queue, state_qe)
          {
            if(this->iTimeout_ < pThread->iTimeout_)
            {
              TAILQ_INSERT_BEFORE(pThread, this, state_qe);
              break;
            }
          }
          // Place at end if no later one found
          if(pThread == NULL)
            TAILQ_INSERT_TAIL(&CTaskManager::timer_queue, this, state_qe);
        }

        // Place task in object wait queue, if object present
        if(this->pWaitObj_ != NULL)
          TAILQ_INSERT_TAIL(&CTaskManager::wait_queue, this, wait_qe);

        break;
      }
    case TS_DEAD:
      TAILQ_INSERT_TAIL(&CTaskManager::dead_queue, this, state_qe);
      break;
    case TS_DESTROY:
      // FIXME:
      //  - Free used resources?
      //  - notify connected threads/tasks?
      //  - free memory?
      //  - Do it here, in destructor or in removeDestroyed?
      break;
    default:
      printk("CThread::state: Warning: unhandled state\n");
    };
  }
}

// -----------------------------------------------------------------------------
CThread *
CThread::createChild(void * entry, int argc, char * argv[])
{
  CThread * pThread = new CThread(pTask_);

  if(pThread != NULL)
  {
    pThread->impl().init(entry, argc, argv);

    TAILQ_INSERT_TAIL(&children_queue, pThread, children_qe);
  }

  return pThread;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CTask::CTask()
 : iPID_(CTaskManager::iPIDCount_++)
 , thr_(this)
{
  memset(pChannel_,        0, sizeof(pChannel_));
  memset(pConnectionsOut_, 0, sizeof(pConnectionsOut_));
  memset(pConnectionsIn_,  0, sizeof(pConnectionsIn_));

  // Insert in the "all tasks" queue
  TAILQ_INSERT_TAIL(&CTaskManager::task_queue, this, task_qe);
}

// -----------------------------------------------------------------------------
CTask::~CTask()
{
}

//------------------------------------------------------------------------------
int
CTask::msgSend(int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

  // User ID
  int iConnectionIDX(CONNECTION_ID_TO_IDX(iConnectionID));

  // Validate out-connection
  if((iConnectionIDX >= 0) &&
     (iConnectionIDX < MAX_OUT_CONNECTION_COUNT) &&
     (pConnectionsOut_[iConnectionIDX] != NULL))
  {
    // Send message
    iRetVal = pConnectionsOut_[iConnectionIDX]->msgSend(pSndMsg, iSndSize, pRcvMsg, iRcvSize);
  }
  else
  {
    printk("CTask::msgSend: Invalid connection id: %d\n", iConnectionID);
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CTask::msgReceive(int iChannelID, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);
  int iChannelIDX(CHANNEL_ID_TO_IDX(iChannelID));

  // Validate channel
  if((iChannelIDX >= 0) &&
     (iChannelIDX < MAX_CHANNEL_COUNT) &&
     (pChannel_[iChannelIDX] != NULL))
  {
    // Receive message
    iRetVal = pChannel_[iChannelIDX]->msgReceive(pRcvMsg, iRcvSize);
  }
  else
  {
    printk("CTask::msgReceive: Invalid channel id: %d\n", iChannelID);
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CTask::msgReply(int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize)
{
  int iRetVal(-1);
  int iConnectionIDX(RCVID_TO_COIDX(iReceiveID));

  // Validate channel
  if((iConnectionIDX >= 0) &&
     (iConnectionIDX < MAX_IN_CONNECTION_COUNT) &&
     (pConnectionsOut_[iConnectionIDX] != NULL))
  {
    iRetVal = pConnectionsIn_[iConnectionIDX]->msgReply(iStatus, pReplyMsg, iReplySize);
  }
  else
  {
    printk("CTask::msgReply: Invalid receive id: %d\n", iReceiveID);
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CTask::addInConnection(IConnection * connection)
{
  int iRetVal(-1);

  // Find empty connection in channel
  for(int iCOIDX(0); iCOIDX < MAX_IN_CONNECTION_COUNT; iCOIDX++)
  {
    if(pConnectionsIn_[iCOIDX] == NULL)
    {
      // Add new connection
      pConnectionsIn_[iCOIDX] = connection;

      // Return connection index
      iRetVal = iCOIDX;

      break;
    }
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CTask::addOutConnection(IConnection * connection)
{
  int iRetVal(-1);

  // Find empty connection in channel
  for(int iCOIDX(0); iCOIDX < MAX_OUT_CONNECTION_COUNT; iCOIDX++)
  {
    if(pConnectionsOut_[iCOIDX] == NULL)
    {
      // Add new connection
      pConnectionsOut_[iCOIDX] = connection;

      // Return connection index
      iRetVal = iCOIDX;

      break;
    }
  }

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CTask::channelRegister(IChannel * channel, unsigned iFlags)
{
  int iRetVal(-1);

  //printk("CTask::channelRegister\n");

  // Locate empty channel
  for(int iChannel(0); iChannel < MAX_CHANNEL_COUNT; iChannel++)
  {
    if(pChannel_[iChannel] == NULL)
    {
      pChannel_[iChannel] = channel;
      iRetVal = CHANNEL_IDX_TO_ID(iChannel);
      break;
    }
  }

  if(iRetVal == -1)
    panic("CTask::channelRegister: Max channels reached!\n");

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CTask::channelCreate(unsigned iFlags)
{
  int iRetVal(-1);

  //printk("CTask::channelCreate\n");

  // Locate empty channel
  for(int iChannel(0); iChannel < MAX_CHANNEL_COUNT; iChannel++)
  {
    if(pChannel_[iChannel] == NULL)
    {
      pChannel_[iChannel] = new CChannelUser;
      iRetVal = CHANNEL_IDX_TO_ID(iChannel);
      break;
    }
  }

  if(iRetVal == -1)
    panic("CTask::channelCreate: Max channels reached!\n");

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CTask::channelDestroy(int iChannelID)
{
  int iRetVal(-1);
  iChannelID = CHANNEL_ID_TO_IDX(iChannelID);

  //printk("CTask::channelDestroy\n");

  if((iChannelID >= 0) &&
     (iChannelID < MAX_CHANNEL_COUNT) &&
     (pChannel_[iChannelID] != NULL))
  {
    // Channel valid, remove it
    delete pChannel_[iChannelID];
    pChannel_[iChannelID] = NULL;
    iRetVal = 0;
  }
  else
  {
    printk("CTask::channelDestroy: Invalid channel id: %d\n", CHANNEL_IDX_TO_ID(iChannelID));
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
int
CTask::channelConnectAttach(uint32_t iNodeID, pid_t iProcessID, int iChannelID, int iFlags)
{
  int iRetVal(-1);

  //printk("CTask::channelConnectAttach\n");

  if(iNodeID == 0)
  {
    iRetVal = channelConnectAttach(iProcessID, iChannelID, iFlags);
  }
  else
  {
    int iNodeIDX = NODEID_TO_NODEIDX(iNodeID);

    if((iNodeIDX >= 0) &&
       (iNodeIDX < MAX_NODES) &&
       (nodes_[iNodeIDX] != NULL))
    {
      // Connect to the gateway
      //iRetVal = channelConnectAttach(nodes_[iNodeIDX]->iProcessID, nodes_[iNodeIDX]->iChannelID, 0);
      if(iRetVal >= 0)
      {
        // Let the gateway connect to the remote node
        iRetVal = nodes_[iNodeIDX]->channelConnectAttach(iProcessID, iChannelID, iFlags);
        if(iRetVal < CONNECTION_ID_BASE)
        {
          printk("CTask::channelConnectAttach: Remote channel(pid-chid) not found(%d-%d)\n", iProcessID, iChannelID);
        }
      }
    }
    else
    {
      printk("CTask::channelConnectAttach: Remote node not found(%d)\n", iNodeID);
    }
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
// Connect to channel on local node
int
CTask::channelConnectAttach(pid_t iProcessID, int iChannelID, int iFlags)
{
  int iRetVal(-1);
  int iChannelIDX = CHANNEL_ID_TO_IDX(iChannelID);
  CTask       * pTask;        // Destination task
  IChannel    * pChannel;     // Destination channel
  IConnection * pConnection;  // New connection to channel

  //printk("CTask::channelConnectAttach\n");

  // Locate task to connect to
  pTask = CTaskManager::getTaskFromPID(iProcessID);
  if(pTask != NULL)
  {
    // Locate channel to connect to
    if((iChannelIDX >= 0) &&
       (iChannelIDX < MAX_CHANNEL_COUNT) &&
       (pTask->pChannel_[iChannelIDX] != NULL))
    {
      pChannel = pTask->pChannel_[iChannelIDX];

      // Connect to the channel
      if(pChannel->createConnection(&pConnection) == true)
      {
        int iCOIDXOut = this->addOutConnection(pConnection);
        if(iCOIDXOut >= 0)
        {
          int iCOIDXIn  = pTask->addInConnection(pConnection);
          if(iCOIDXIn >= 0)
          {
            // Receive ID: Index into the tasks incomming-connection array
            pConnection->setReceiveID(COIDX_TO_RCVID(iCOIDXIn));

            // Connection ID: Index into the tasks outgoing-connection array
            iRetVal = CONNECTION_IDX_TO_ID(iCOIDXOut);
          }
          else
          {
            printk("CTask::channelConnectAttach: Unable to add in-connection to task\n");
          }
        }
        else
        {
          printk("CTask::channelConnectAttach: Unable to add out-connection to task\n");
        }
      }
      else
      {
        printk("CTask::channelConnectAttach: Unable to connect to channel\n");
      }
    }
    else
    {
      printk("CTask::channelConnectAttach: Channel id not found: %d\n", iChannelID);
    }
  }
  else
  {
    printk("CTask::channelConnectAttach: Process id not found: %d\n", iProcessID);
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
int
CTask::channelConnectDetach(int iConnectionID)
{
  int iRetVal(-1);
  iConnectionID = CONNECTION_ID_TO_IDX(iConnectionID);

  //printk("CTask::channelConnectDetach\n");

  if((iConnectionID >= 0) &&
     (iConnectionID < MAX_OUT_CONNECTION_COUNT) &&
     (pConnectionsOut_[iConnectionID] != NULL))
  {
    pConnectionsOut_[iConnectionID]->disconnect();
    delete pConnectionsOut_[iConnectionID];
    pConnectionsOut_[iConnectionID] = NULL;
    iRetVal = 0;
  }
  else
  {
    printk("CTask::channelConnectDetach: Connection id not found: %d\n", CONNECTION_IDX_TO_ID(iConnectionID));
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
CTaskManager::updateSleepers()
{
  CThread * pThread;

  //printk("CTaskManager::updateSleepers\n");

  // FIXME! This only works with 100Hz scheduler (10000us/10ms interval)
  iCurrentTime_ += 10000;
  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH(pThread, &timer_queue, state_qe)
  {
    // We can leave the loop if a not timed out task is found since the list
    // is sorted.
    if(iCurrentTime_ < pThread->iTimeout_)
      break;

    // Time out! Task is ready to run!
    if(pThread->pWaitObj_ != NULL)
      pThread->iWaitReturn_ = -1;
    else
      pThread->iWaitReturn_ = 0;
    pThread->state(TS_READY);
  }
}

// -----------------------------------------------------------------------------
void
CTaskManager::removeDestroyed()
{
  CThread * pThread;
  CThread * pSafe;

  //printk("CTaskManager::removeDestroyed\n");

  // Wake up sleaping tasks if timeout exeeded
  TAILQ_FOREACH_SAFE(pThread, &dead_queue, state_qe, pSafe)
  {
    pThread->state(TS_DESTROY);
  }
}

// -----------------------------------------------------------------------------
bool
CTaskManager::schedule()
{
  CThread * pPrevThread = pCurrentThread_;

  //printk("CTaskManager::schedule\n");

  // Run the first task in the ready queue
  if(TAILQ_FIRST(&ready_queue) != NULL)
    TAILQ_FIRST(&ready_queue)->state(TS_RUNNING);

  if(pCurrentThread_ == NULL)
  {
    if(pIdleThread_ != NULL)
      pCurrentThread_ = pIdleThread_;
    else
      panic("CTaskManager::schedule: ERROR: No task to run\n");
  }

  return pPrevThread != pCurrentThread_;
}

// -----------------------------------------------------------------------------
CTask *
CTaskManager::getTaskFromPID(pid_t pid)
{
  CTask * pTask;

  TAILQ_FOREACH(pTask, &task_queue, task_qe)
    if(pTask->iPID_ == pid)
      return pTask;

  return NULL;
}
