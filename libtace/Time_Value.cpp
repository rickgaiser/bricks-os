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


#include "ace/Time_Value.h"


#define ACE_ONE_SECOND_IN_USECS 1000000


//---------------------------------------------------------------------------
ACE_Time_Value::ACE_Time_Value()
{
  this->set(0, 0);
}

//---------------------------------------------------------------------------
ACE_Time_Value::ACE_Time_Value(long sec, long usec)
{
  this->set(sec, usec);
}

//---------------------------------------------------------------------------
ACE_Time_Value::ACE_Time_Value(const struct timeval & t)
{
  this->set(t);
}

//---------------------------------------------------------------------------
void
ACE_Time_Value::set(long sec, long usec)
{
  this->tv_.tv_sec = sec;
  this->tv_.tv_usec = usec;
  this->normalize ();
}

//---------------------------------------------------------------------------
void
ACE_Time_Value::set(const timeval & tv)
{
  this->tv_.tv_sec = tv.tv_sec;
  this->tv_.tv_usec = tv.tv_usec;
  this->normalize ();
}

//---------------------------------------------------------------------------
unsigned long
ACE_Time_Value::msec() const
{
  return this->tv_.tv_sec * 1000 + this->tv_.tv_usec / 1000;
}

//---------------------------------------------------------------------------
void
ACE_Time_Value::msec(long milliseconds)
{
  this->tv_.tv_sec = milliseconds / 1000;
  this->tv_.tv_usec = (milliseconds - (this->tv_.tv_sec * 1000)) * 1000;
}

//---------------------------------------------------------------------------
void
ACE_Time_Value::normalize()
{
  if(this->tv_.tv_usec >= ACE_ONE_SECOND_IN_USECS)
  {
    do
    {
      this->tv_.tv_sec++;
      this->tv_.tv_usec -= ACE_ONE_SECOND_IN_USECS;
    }
    while(this->tv_.tv_usec >= ACE_ONE_SECOND_IN_USECS);
  }
  else if(this->tv_.tv_usec <= -ACE_ONE_SECOND_IN_USECS)
  {
    do
    {
      this->tv_.tv_sec--;
      this->tv_.tv_usec += ACE_ONE_SECOND_IN_USECS;
    }
    while(this->tv_.tv_usec <= -ACE_ONE_SECOND_IN_USECS);
  }

  if(this->tv_.tv_sec >= 1 && this->tv_.tv_usec < 0)
  {
    this->tv_.tv_sec--;
    this->tv_.tv_usec += ACE_ONE_SECOND_IN_USECS;
  }
  else if(this->tv_.tv_sec < 0 && this->tv_.tv_usec > 0)
  {
    this->tv_.tv_sec++;
    this->tv_.tv_usec -= ACE_ONE_SECOND_IN_USECS;
  }
}
