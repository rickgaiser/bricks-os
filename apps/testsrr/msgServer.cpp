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


#include "msgServer.h"
#include "kernel/debug.h"
#include "kernel/srr.h"
#include "unistd.h"


//---------------------------------------------------------------------------
CMsgServer::CMsgServer(const char * name)
 : sName_(name)
 , iChannelID_(0)
{
  iPID_ = getpid();
}

//---------------------------------------------------------------------------
CMsgServer::~CMsgServer()
{
}

//---------------------------------------------------------------------------
int
CMsgServer::svc()
{
  iChannelID_ = channelCreate(0);
  if(iChannelID_ > 1)
  {
    int rcvid;
    char recvBuffer[80];

    // Register name is one was passed
    if(sName_ != 0)
    {
      if(registerName(iChannelID_, sName_) < 0)
        printk("CMsgServer::svc: ERROR: Unable to register name\n");
    }

    while(true)
    {
      rcvid = msgReceive(iChannelID_, recvBuffer, 80);
//      if(rcvid <= 0)
//        break;
      if(process(rcvid, recvBuffer) < 0)
        break;
    }

    channelDestroy(iChannelID_);
  }
  else
    printk("CMsgServer::svc: ERROR: Unable to get channel\n");

  return 0;
}

//---------------------------------------------------------------------------
int
CMsgServer::getPID() const
{
  return iPID_;
}

//---------------------------------------------------------------------------
int
CMsgServer::getChannelID() const
{
  return iChannelID_;
}
