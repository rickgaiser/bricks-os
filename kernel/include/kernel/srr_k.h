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
