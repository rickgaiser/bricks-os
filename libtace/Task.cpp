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
#include "stdio.h"


//---------------------------------------------------------------------------
ACE_Task_Base::ACE_Task_Base()
{
  for(int i(0); i < MAX_THREADS; i++)
  {
    thread_[i].used = false;
  }
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
ACE_Task_Base::activate(int nthreads)
{
  for(int i(0); (i < MAX_THREADS) && (i < nthreads); i++)
  {
    pthread_create(&thread_[i].thr, 0, (void *(*)(void *))this->svc_run, this);
    thread_[i].used = true;
  }

  return 0;
}

//---------------------------------------------------------------------------
int
ACE_Task_Base::wait()
{
  void * status;

  for(int i(0); i < MAX_THREADS; i++)
  {
    if(thread_[i].used == true)
    {
      pthread_join(thread_[i].thr, &status);
      thread_[i].used = false;
    }
  }

  return 0;
}

//---------------------------------------------------------------------------
ACE_THR_FUNC_RETURN
ACE_Task_Base::svc_run(void * args)
{
  ACE_THR_FUNC_RETURN retval = -1;

  if(args != NULL)
  {
    ACE_Task_Base * t = (ACE_Task_Base *)args;

    retval = t->svc();
  }
  else
    printf("ACE_Task_Base::svc_run: ERROR: NULL pointer received\n");

  pthread_exit((void *)retval);

  return retval;
}
