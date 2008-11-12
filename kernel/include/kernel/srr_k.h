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


#ifndef KERNEL_SRR_H
#define KERNEL_SRR_H


#include "kernel/syscall.h"
#include "sys/types.h"


#define CHANNEL_ID_BASE         0x1000
#define CHANNEL_IDX_TO_ID(i)    ((i)+CHANNEL_ID_BASE)
#define CHANNEL_ID_TO_IDX(i)    ((i)-CHANNEL_ID_BASE)

#define CONNECTION_ID_BASE      0x1000
#define CONNECTION_IDX_TO_ID(i) ((i)+CONNECTION_ID_BASE)
#define CONNECTION_ID_TO_IDX(i) ((i)-CONNECTION_ID_BASE)


#ifdef __cplusplus
extern "C" {
#endif


// SRR
declareSysCallKernel5r(int, msgSend,    int, iConnectionID, const void *, pSndMsg, int, iSndSize, void *, pRcvMsg, int, iRcvSize);
declareSysCallKernel3r(int, msgReceive, int, iChannelID, void *, pRcvMsg, int, iRcvSize);
declareSysCallKernel4r(int, msgReply,   int, iReceiveID, int, iStatus, const void *, pReplyMsg, int, iReplySize);
// Channels
declareSysCallKernel1r(int, channelCreate, unsigned, iFlags);
declareSysCallKernel1r(int, channelDestroy, int, iChannelID);
declareSysCallKernel4r(int, channelConnectAttach, uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
declareSysCallKernel1r(int, channelConnectDetach, int, iConnectionID);
// Names
declareSysCallKernel2r(int, registerName, int, channelID, const char *, name);
declareSysCallKernel3r(int, lookupName,   const char *, name, int *, pid, int *, channelID);
// Nodes
declareSysCallKernel2r(int, registerNodeGateway, int, channelIDControl, int, channelIDData);


#ifdef __cplusplus
}
#endif


#endif
