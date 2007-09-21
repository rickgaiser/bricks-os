#ifndef TASK_H
#define TASK_H


#include "kernel/fs.h"
#include "kernel/queue.h"
#include "inttypes.h"


#define MAX_CHANNEL_COUNT    10
#define MAX_CONNECTION_COUNT 10


// -----------------------------------------------------------------------------
enum EThreadState
{
  TS_UNINITIALIZED,
  TS_READY,
  TS_RUNNING,
  TS_WAITING,
  TS_DEAD,
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

class CTask;
class CThread;
TAILQ_HEAD(STaskQueue, CTask);
TAILQ_HEAD(SThreadQueue, CThread);
// -----------------------------------------------------------------------------
class CThread
{
public:
  virtual ~CThread();

  // Task switch #1: Jump to task immediately.
  //  - Used from caller context
  virtual void runJump();
  // Task switch #2: Setup stack so interrupt return will couse this task to run.
  //  - Used from interrupt context
  virtual void runReturn();

  void state(EThreadState state);

  CThread * createChild(void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);

  uint32_t iTimeout_;                                // Timeout in us
  void * pWaitObj_;                                  // Object we're waiting on
  int iWaitReturn_;                                  // Return value when waiting

  TAILQ_ENTRY(CThread) thread_qe;                    // All threads queue entry
  TAILQ_ENTRY(CThread) state_qe;                     // Thread state queue entry
  TAILQ_ENTRY(CThread) wait_qe;                      // Thread wait queue entry

  // Tree structure of threads
  CTask * pTask_;
  CThread * pParent_;
  SThreadQueue children_queue;                       // Threads children queue
  TAILQ_ENTRY(CThread) children_qe;                  // Threads children queue entry

protected:
  CThread(CTask * task);

private:
  EThreadState eState_;
};

// -----------------------------------------------------------------------------
class CTask
{
public:
  CTask(void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CTask();

  CThread * thr_;

  SChannel * pChannel_[MAX_CHANNEL_COUNT];           // Tasks Channels
  SChannel * pConnection_[MAX_CONNECTION_COUNT];     // Tasks Connections

  pid_t iPID_;

  TAILQ_ENTRY(CTask) task_qe;                        // All tasks queue
};

// -----------------------------------------------------------------------------
class CTaskManager
{
public:
  static void updateSleepers();
  static void removeDestroyed();
  static bool schedule();
  static CTask * getTaskFromPID(pid_t pid);

  static CTask * pCurrentTask_;
  static CThread * pCurrentThread_;
  static STaskQueue task_queue;        // All tasks
  static SThreadQueue thread_queue;    // All threads
  static SThreadQueue ready_queue;     // Ready to run threads
  static SThreadQueue timer_queue;     // Sleeping on timer threads
  static SThreadQueue wait_queue;      // Sleeping on objects
  static SThreadQueue dead_queue;      // Ready to be destroyed
  static uint32_t iPIDCount_;
  static useconds_t iCurrentTime_;

private:
  CTaskManager(){}
};


int genwait_wait(void * obj, useconds_t useconds);
int genwait_wake(void * obj, int maxcount);

extern "C" pid_t   k_getpid(void);
extern "C" int     k_usleep(useconds_t useconds);


#endif
