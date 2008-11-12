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
CConnectionUser::CConnectionUser(CChannelUser * pChannel)
 : channel_(pChannel)
 , iReceiveID_(-1)
 , iState_(COS_FREE)
{
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_cond_init(&stateCond_, NULL);
}

//------------------------------------------------------------------------------
CConnectionUser::~CConnectionUser()
{
  if(channel_ != NULL)
  {
    channel_->removeConnection(this);
  }
}

//------------------------------------------------------------------------------
void
CConnectionUser::disconnect()
{
  // Wait for connection to become free
  k_pthread_mutex_lock(&mutex_);
  while(iState_ != COS_FREE)
    k_pthread_cond_wait(&stateCond_, &mutex_);

  if(channel_ != NULL)
  {
    channel_->removeConnection(this);

    channel_    = NULL;
    iReceiveID_ = -1;
  }

  // Free the connection
  k_pthread_mutex_unlock(&mutex_);
}

//------------------------------------------------------------------------------
void
CConnectionUser::setReceiveID(int iReceiveID)
{
  iReceiveID_ = iReceiveID;
}

//------------------------------------------------------------------------------
int
CConnectionUser::getReceiveID()
{
  return iReceiveID_;
}

//------------------------------------------------------------------------------
int
CConnectionUser::msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

  //printk("CConnectionUser::msgSend\n");

  // Lock the connection
  k_pthread_mutex_lock(&mutex_);

  if(channel_ != NULL)
  {
    // Wait for connection to become free
    while(iState_ != COS_FREE)
      k_pthread_cond_wait(&stateCond_, &mutex_);

    // Set message ready
    pSndMsg_  = pSndMsg;
    iSndSize_ = iSndSize;
    pRcvMsg_  = pRcvMsg;
    iRcvSize_ = iRcvSize;
    // Send message to channel
    channel_->msgSend(*this);

    // Wait for reply
    iState_ = COS_MSG_SENT;
    k_pthread_cond_broadcast(&stateCond_);
    while(iState_ != COS_MSG_REPLIED)
      k_pthread_cond_wait(&stateCond_, &mutex_);

    iRetVal = iRetVal_;

    // Free the connection
    iState_ = COS_FREE;
    k_pthread_cond_broadcast(&stateCond_);
  }
  else
  {
    printk("CConnectionUser::msgSend: ERROR: Not connected\n");
  }

  // Unlock the connection
  k_pthread_mutex_unlock(&mutex_);

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CConnectionUser::msgReply(int iStatus, const void * pReplyMsg, int iReplySize)
{
  int iRetVal(-1);

  //printk("CConnectionUser::msgReply\n");

  // Lock the connection
  k_pthread_mutex_lock(&mutex_);

  if(channel_ != NULL)
  {
    if(iState_ == COS_MSG_SENT)
    {
      // Copy reply data and return status
      if(iReplySize < iRcvSize_)
        iReplySize = iRcvSize_;
      memcpy(pRcvMsg_, pReplyMsg, iReplySize);
      iRetVal_ = iStatus;

      iRetVal = 0;

      // Notify this->msgSend of replied message
      iState_ = COS_MSG_REPLIED;
      k_pthread_cond_broadcast(&stateCond_);
    }
    else
    {
      printk("CConnectionUser::msgReply: Can't reply, no message sent\n");
    }
  }
  else
  {
    printk("CConnectionUser::msgReply: ERROR: Not connected\n");
  }

  // Unlock the connection
  k_pthread_mutex_unlock(&mutex_);

  return iRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CConnectionKernel::CConnectionKernel(CAChannelKernel * pChannel)
 : channel_(pChannel)
{
  k_pthread_mutex_init(&mutex_, NULL);
}

//------------------------------------------------------------------------------
CConnectionKernel::~CConnectionKernel()
{
  if(channel_ != NULL)
  {
    channel_->removeConnection(this);
  }
}

//------------------------------------------------------------------------------
void
CConnectionKernel::disconnect()
{
  // Wait for connection to become free
  k_pthread_mutex_lock(&mutex_);

  if(channel_ != NULL)
  {
    channel_->removeConnection(this);
    channel_ = NULL;
  }

  // Free the connection
  k_pthread_mutex_unlock(&mutex_);
}

//------------------------------------------------------------------------------
void
CConnectionKernel::setReceiveID(int iReceiveID)
{
}

//------------------------------------------------------------------------------
int
CConnectionKernel::getReceiveID()
{
  return -1;
}

//------------------------------------------------------------------------------
int
CConnectionKernel::msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize)
{
  int iRetVal(-1);

  //printk("CConnectionKernel::msgSend\n");

  // Lock the connection
  k_pthread_mutex_lock(&mutex_);

  if(channel_ != NULL)
  {
    // Send message to channel
    iRetVal = channel_->msgSend(pSndMsg, iSndSize, pRcvMsg, iRcvSize);
  }
  else
  {
    printk("CConnectionKernel::msgSend: ERROR: Not connected\n");
  }

  // Unlock the connection
  k_pthread_mutex_unlock(&mutex_);

  return iRetVal;
}

//------------------------------------------------------------------------------
int
CConnectionKernel::msgReply(int iStatus, const void * pReplyMsg, int iReplySize)
{
  panic("CConnectionKernel::msgReply: Can't reply on kernel connection\n");

  return -1;
}
