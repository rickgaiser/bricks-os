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


#ifndef KERNEL_SRRNODES_H
#define KERNEL_SRRNODES_H


#include "kernel/syscall.h"
#include "kernel/srrChannel.h"
#include "sys/types.h"
#include "stdint.h"


#define MAX_NODES         10
#define NODEID_TO_NODEIDX(id)  ((id)-1)
#define NODEIDX_TO_NODEID(idx) ((idx)+1)


class CGateway
{
public:
  CGateway(int iPID, int iChannelIDControl, int iChannelIDData);
  virtual ~CGateway();

  int channelConnectAttach(pid_t iProcessID, int iChannelID, int iFlags);

private:
  // Connection to the control channel
  IConnection * ctrl_;

  int iPID_;
  int iChannelIDControl_;
  int iChannelIDData_;
};


extern CGateway * nodes_[MAX_NODES];


#endif
