#ifndef TASK_H
#define TASK_H


#include "asm/irq.h"
#include "kernel/fs.h"
#include "inttypes.h"


#define MAX_TASK_COUNT 10
#define MAX_FILE_COUNT 10


// -----------------------------------------------------------------------------
enum ETaskState
{
  TS_UNINITIALIZED,
  TS_RUNNING,
  TS_READY,
  TS_SEND_BLOCKED,
  TS_RECEIVE_BLOCKED,
  TS_REPLY_BLOCKED
};

// -----------------------------------------------------------------------------
class CTask
{
public:
  CTask(void * entry, size_t stack, size_t svcstack, int argc = 0, char * argv[] = 0);
  virtual ~CTask();

  static void addTask(CTask * pTask);
  static void setStandardInput(IFileIO * pSTDIN);
  static void setStandardOutput(IFileIO * pSTDOUT);

  static uint32_t   iTaskCount_;
  static CTask    * pCurrentTask_;
  static IFileIO  * pSTDIN_;
  static IFileIO  * pSTDOUT_;

  IFileIO    * pFiles_[MAX_FILE_COUNT];
  ETaskState   eState_;
  pt_regs    * pTaskState_;
  uint32_t   * pStack_;
  uint32_t   * pSvcStack_;

  CTask      * prev;
  CTask      * next;
};


#endif
