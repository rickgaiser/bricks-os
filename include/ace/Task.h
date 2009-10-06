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


#ifndef ACE_TASK_H
#define ACE_TASK_H


#include "ace/config.h"
//#include "ace/Service_Object.h"
#include "pthread.h"


#define MAX_THREADS 16


class ACE_Task_Base
// : public ACE_Service_Object
{
public:
  ACE_Task_Base();
  virtual ~ACE_Task_Base();

  virtual int svc();
  virtual int activate(int nthreads = 1);
  virtual int wait();

  static ACE_THR_FUNC_RETURN svc_run(void *);

private:
  struct SThread
  {
    bool used;
    pthread_t thr;
  };

  SThread thread_[MAX_THREADS];
};


#endif
