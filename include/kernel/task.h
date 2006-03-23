#ifndef TASK_H
#define TASK_H


#include "memoryMap.h"
#include "file.h"


#define MAX_FILES 20


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
  CTask();
  virtual ~CTask();

  CMemoryMap * memoryMap();
  void memoryMap(CMemoryMap * pMemoryMap);

  CFileHandle * pFiles_[MAX_FILES];

public: // but should be private
  ETaskState    eState_;
private:
  CMemoryMap  * pMemoryMap_;
};


#endif
