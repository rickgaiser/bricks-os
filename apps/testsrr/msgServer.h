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
 */


#ifndef MSGSERVER_H
#define MSGSERVER_H


#include "ace/Task.h"


//---------------------------------------------------------------------------
class CMsgServer
 : public ACE_Task_Base
{
public:
  CMsgServer(const char * name = 0);
  virtual ~CMsgServer();

  int svc();
  virtual int process(int iReceiveID, void * pRcvMsg) = 0;

  int getPID() const;
  int getChannelID() const;

protected:
  const char * sName_;
  int iPID_;
  int iChannelID_;
};


#endif
