#ifndef ACE_THREAD_MANAGER_H
#define ACE_THREAD_MANAGER_H


#include "ace/config.h"


class ACE_Thread_Manager
{
public:
  ACE_Thread_Manager();
  virtual ~ACE_Thread_Manager();

  static ACE_Thread_Manager * instance();

  int spawn(ACE_THR_FUNC func, void * args = 0);

private:
  static ACE_Thread_Manager * thr_mgr_;
};


#endif
