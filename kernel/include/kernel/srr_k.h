#ifndef KERNEL_SRR_H
#define KERNEL_SRR_H


#ifdef __cplusplus
extern "C" {
#endif


int k_msgSend   (int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);
int k_msgReceive(int iChannelID, void * pRcvMsg, int iRcvSize);
int k_msgReply  (int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize);


#ifdef __cplusplus
}
#endif


#endif
