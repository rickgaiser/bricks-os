#ifndef TASK_H
#define TASK_H


#include "kernel/fs.h"
#include "kernel/queue.h"
#include "inttypes.h"


#define MAX_CHANNEL_COUNT    10
#define MAX_CONNECTION_COUNT 10


// -----------------------------------------------------------------------------
enum ETaskState
{
  TS_UNINITIALIZED,
  TS_READY,
  TS_RUNNING,
  TS_WAITING,
  TS_DESTROY,
  TS_SEND_BLOCKED,
  TS_RECEIVE_BLOCKED,
  TS_REPLY_BLOCKED
};

// -----------------------------------------------------------------------------
enum EChannelState
{
  CS_FREE,
  CS_USED,
  CS_MSG_SEND,
  CS_MSG_RECEIVED,
  CS_MSG_REPLIED
};

// -----------------------------------------------------------------------------
struct SChannel
{
  volatile int iState;
  const void * pSndMsg;
  int iSndSize;
  void * pRcvMsg;
  int iRcvSize;
  int iRetVal;
};

// -----------------------------------------------------------------------------
struct SConnection
{
  SChannel * channel;
};

// -----------------------------------------------------------------------------
class CTask
{
public:
  virtual ~CTask();

  // Switch tasks! Function can do two things:
  //  - Setup stack so interrupt return will couse this task to run.
  //  - Jump to task immediately.
  virtual void run() = 0;

  SChannel * pChannel_[MAX_CHANNEL_COUNT];           // Tasks Channels
  SChannel * pConnection_[MAX_CONNECTION_COUNT];     // Tasks Connections

  ETaskState eState_;                                // Current task state
  uint32_t iTimeout_;                                // Timeout in us
  pid_t iPID_;

  TAILQ_ENTRY(CTask) task_queue;                     // All tasks queue
  TAILQ_ENTRY(CTask) state_queue;                    // Queue for current tasks state

protected:
  CTask();
};

TAILQ_HEAD(STaskQueue, CTask);
// -----------------------------------------------------------------------------
class CTaskManager
{
public:
  static void addTask(CTask * pTask);
  static void removeTask(CTask * pTask);
  static bool schedule();

  static CTask * pCurrentTask_;
  static STaskQueue task_queue;  // All tasks queue
  static STaskQueue run_queue;   // Runnable tasks
  static uint32_t iPIDCount_;

private:
  CTaskManager(){}
};


extern "C" pid_t   k_getpid(void);
extern "C" int     k_usleep(useconds_t useconds);


// -----------------------------------------------------------------------------
// To be implemeted in arch
extern CTask * getNewTask(void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);


#endif
