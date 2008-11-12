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
 *
 * Based on the Adaptive Communication Environment (ACE)
 */


#include "ace/Task.h"
#include "kernel/debug.h"


//---------------------------------------------------------------------------
ACE_Task_Base::ACE_Task_Base(ACE_Thread_Manager * thr_man)
 : thr_mgr_(thr_man)
{
}

//---------------------------------------------------------------------------
ACE_Task_Base::~ACE_Task_Base()
{
}

//---------------------------------------------------------------------------
int
ACE_Task_Base::svc()
{
  return 0;
}

//---------------------------------------------------------------------------
int
ACE_Task_Base::activate()
{
  return pthread_create(&thr_, 0, (void *(*)(void *))this->svc_run, this);
}

//---------------------------------------------------------------------------
int
ACE_Task_Base::wait()
{
  void * status;

  return pthread_join(thr_, &status);
}

//---------------------------------------------------------------------------
ACE_Thread_Manager *
ACE_Task_Base::thr_mgr() const
{
  return this->thr_mgr_;
}

//---------------------------------------------------------------------------
void
ACE_Task_Base::thr_mgr(ACE_Thread_Manager * thr_mgr)
{
  this->thr_mgr_ = thr_mgr;
}

//---------------------------------------------------------------------------
ACE_THR_FUNC_RETURN
ACE_Task_Base::svc_run(void * args)
{
  ACE_THR_FUNC_RETURN iRetVal = -1;

  if(args != NULL)
    iRetVal = ((ACE_Task_Base *)args)->svc();
  else
    printk("ACE_Task_Base::svc_run: ERROR: NULL pointer received\n");

  pthread_exit((void *)iRetVal);

  return iRetVal;
}
