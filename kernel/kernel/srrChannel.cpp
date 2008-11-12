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


#include "kernel/debug.h"
#include "kernel/srr_k.h"
#include "kernel/srrChannel.h"
#include "kernel/srrConnection.h"
#include "kernel/task.h"
#include "string.h"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CAChannel::CAChannel()
{
  memset(pConnectionsIn_, 0, sizeof(pConnectionsIn_));
}

//------------------------------------------------------------------------------
CAChannel::~CAChannel()
{
  // FIXME: Connections need to be disconnected!
}

//------------------------------------------------------------------------------
int
CAChannel::addConnection(IConnection * connection)
{
  int iRetVal(-1);

  //printk("CAChannel::addConnection\n");

  // Find empty connection in channel
  for(int iCOIDX(0); iCOIDX < MAX_IN_CONNECTION_COUNT; iCOIDX++)
  {
    if(pConnectionsIn_[iCOIDX] == NULL)
    {
      // Add new connection
      pConnectionsIn_[iCOIDX] = connection;

      // Return connection index
      iRetVal = iCOIDX;

      break;
    }
  }

  if(iRetVal < 0)
    printk("CChannelUser::addConnection: ERROR: Max connections reached\n");

  return iRetVal;
}

//------------------------------------------------------------------------------
void
CAChannel::removeConnection(IConnection * connection)
{
  for(int iCOIDX(0); iCOIDX < MAX_IN_CONNECTION_COUNT; iCOIDX++)
    if(pConnectionsIn_[iCOIDX] == connection)
      pConnectionsIn_[iCOIDX] = NULL;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CChannelUser::CChannelUser()
 : CAChannel()
 , pMsgWaiting_(NULL)
 , iState_(CHS_FREE)
{
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_cond_init(&stateCond_, NULL);
}

//------------------------------------------------------------------------------
CChannelUser::~CChannelUser()
{
}

//------------------------------------------------------------------------------
bool
CChannelUser::createConnection(IConnection ** connection)
{
  *connection = new CConnectionUser(this);
  if(*connection != NULL)
  {
    this->addConnection(*connection);
    return true;
  }
  else
  {
    panic("Out of memory!\n");
    return true;
  }
}

//------------------------------------------------------------------------------
void
CChannelUser::msgSend(CConnectionUser & connection)
{
  //printk("CChannelUser::msgSend\n");

  // Wait for channel to become free
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != CHS_FREE)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  // Set the waiting message, and notify msgReceive of the new message
  pMsgWaiting_ = &connection;
  iState_ = CHS_MSG_SENT;
  k_pthread_cond_broadcast(&stateCond_);
  k_pthread_mutex_unlock(&mutex_);
}

//------------------------------------------------------------------------------
int
CChannelUser::msgReceive(void * pRcvMsg, int iRcvSize)
{
  int iRetVal;

  //printk("CChannelUser::msgReceive\n");

  // Wait for a message
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != CHS_MSG_SENT)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  // Copy the data and receive id
  if(iRcvSize > pMsgWaiting_->iSndSize_)
    iRcvSize = pMsgWaiting_->iSndSize_;
  memcpy(pRcvMsg, pMsgWaiting_->pSndMsg_, iRcvSize);
  iRetVal = pMsgWaiting_->iReceiveID_;

  // Release the channel, so the next message can be sent
  iState_ = CHS_FREE;
  k_pthread_cond_broadcast(&stateCond_);
  k_pthread_mutex_unlock(&mutex_);

  return iRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CAChannelKernel::CAChannelKernel()
 : CAChannel()
{
}

//------------------------------------------------------------------------------
CAChannelKernel::~CAChannelKernel()
{
}

//------------------------------------------------------------------------------
bool
CAChannelKernel::createConnection(IConnection ** connection)
{
  *connection = new CConnectionKernel(this);
  if(*connection != NULL)
  {
    this->addConnection(*connection);
    return true;
  }
  else
  {
    panic("Out of memory!\n");
    return true;
  }
}

//------------------------------------------------------------------------------
int
CAChannelKernel::msgReceive(void * pRcvMsg, int iRcvSize)
{
  panic("CChannelKernel::msgReceive: Can't receive on kernel channel\n");

  return -1;
}
