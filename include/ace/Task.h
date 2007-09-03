#ifndef ACE_TASK_H
#define ACE_TASK_H


//#include "ace/Service_Object.h"
#include "ace/Thread_Manager.h"
#include <stddef.h>


class ACE_Task_Base
// : public ACE_Service_Object
{
public:
  ACE_Task_Base(ACE_Thread_Manager * = 0);
  virtual ~ACE_Task_Base();

  virtual int svc();
  virtual int activate();
  virtual int wait();

  ACE_Thread_Manager * thr_mgr() const;
  void thr_mgr (ACE_Thread_Manager *);

  static ACE_THR_FUNC_RETURN svc_run(void *);

private:
  size_t thr_count_;
  ACE_Thread_Manager * thr_mgr_;
};


#endif
