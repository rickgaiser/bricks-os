#ifndef KERNEL_SRR_H
#define KERNEL_SRR_H


#include "kernel/syscall.h"


#define CHANNEL_ID_BASE         0x1000
#define CHANNEL_IDX_TO_ID(i)    ((i)+CHANNEL_ID_BASE)
#define CHANNEL_ID_TO_IDX(i)    ((i)-CHANNEL_ID_BASE)

#define CONNECTION_ID_BASE      0x1000
#define CONNECTION_IDX_TO_ID(i) ((i)+CONNECTION_ID_BASE)
#define CONNECTION_ID_TO_IDX(i) ((i)-CONNECTION_ID_BASE)


#ifdef __cplusplus
extern "C" {
#endif


declareSysCallKernel5r(int, msgSend,    int, iConnectionID, const void *, pSndMsg, int, iSndSize, void *, pRcvMsg, int, iRcvSize);
declareSysCallKernel3r(int, msgReceive, int, iChannelID, void *, pRcvMsg, int, iRcvSize);
declareSysCallKernel4r(int, msgReply,   int, iReceiveID, int, iStatus, const void *, pReplyMsg, int, iReplySize);


#ifdef __cplusplus
}
#endif


#endif
