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


#ifndef KERNEL_SRRCONNECTION_H
#define KERNEL_SRRCONNECTION_H


#include "kernel/syscall.h"
#include "kernel/fs.h"
#include "sys/types.h"
#include "stdint.h"


// -----------------------------------------------------------------------------
enum EConnectionState
{
  COS_FREE,
  COS_MSG_SENT,
  COS_MSG_REPLIED,
};

// -----------------------------------------------------------------------------
// Early declarations
class IChannel;
class CChannelUser;
class CAChannelKernel;

// -----------------------------------------------------------------------------
class IConnection
{
public:
  virtual ~IConnection(){}

  virtual void disconnect() = 0;

  virtual void setReceiveID(int iReceiveID) = 0;
  virtual int  getReceiveID() = 0;

  virtual int  msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize) = 0;
  virtual int  msgReply(int iStatus, const void * pReplyMsg, int iReplySize) = 0;
};

// -----------------------------------------------------------------------------
class CConnectionUser
 : public IConnection
{
  friend class CChannelUser;

public:
  virtual ~CConnectionUser();

  void disconnect();

  void setReceiveID(int iReceiveID);
  int  getReceiveID();

  int  msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);
  int  msgReply(int iStatus, const void * pReplyMsg, int iReplySize);

private:
  CConnectionUser(CChannelUser * pChannel);

private:
  CChannelUser * channel_;
  int iReceiveID_;              // msgReceive will return this id

  pthread_mutex_t mutex_;       // Mutex to notify us of a replied message
  pthread_cond_t stateCond_;    // State change condition
  int iState_;                  // Current state

  const void * pSndMsg_;        // Location of the sent message
  int iSndSize_;                // Size of the sent message
  void * pRcvMsg_;              // Location where return message can be stored
  int iRcvSize_;                // Size of the return message buffer
  int iRetVal_;                 // Return value for the msgSend function
};

// -----------------------------------------------------------------------------
class CConnectionKernel
 : public IConnection
{
  friend class CAChannelKernel;

public:
  virtual ~CConnectionKernel();

  void disconnect();

  void setReceiveID(int iReceiveID);
  int  getReceiveID();

  int  msgSend(const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);
  int  msgReply(int iStatus, const void * pReplyMsg, int iReplySize);

private:
  CConnectionKernel(CAChannelKernel * pChannel);

private:
  CAChannelKernel * channel_;
  pthread_mutex_t mutex_;
};


#endif
