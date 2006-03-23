#ifndef TASKMANAGER_H
#define TASKMANAGER_H


#include "task.h"
#include "eventHandler.h"
#include "archTimer.h"


#define MAX_TASK_COUNT 10


/**
 * \brief Task Manager class is the owner of all tasks (CTask objects)
 *
 * It is responsible for:
 *   - creation (of tasks)
 *   - removal  (of tasks)
 *   - scheduling
 */
class CTaskManager
 : public IEventHandler
{
public:
  static CTaskManager * instance();  

  int init();

  CTask * getCurrentTask();

  // Derived from IEventHandler
  virtual int handle_timeout(const CTimeValue & tv, const void * arg = 0);

private:
  CTaskManager();
  virtual ~CTaskManager();

  static CTaskManager   singleton_;               // Task Manager Singleton

  // Untill memory management is complete we use a static nr of tasks.
  CTask    task_[MAX_TASK_COUNT];
  int      iCurrentTaskIndex_;
  CTask  * pCurrentTask_;
  ITimer * pTimer_;
};


#endif
