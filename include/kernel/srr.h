#ifndef KERNEL_SRR_H
#define KERNEL_SRR_H


// Connection Ranges:
//  - Static range for kernel predefined channels
//  - Dynamic range for runtime assignment
#define STATIC_CONNECTIONS_START            0
#define STATIC_CONNECTIONS_SIZE      0x100000
#define DYNAMIC_CONNECTIONS_START    0x100000
#define DYNAMIC_CONNECTIONS_SIZE   0x7ff00000


// Predefined Channels
enum EStaticConnectionID
{
    scERROR  = 0  // Error detection for connection ID == 0
  , scKERNEL = 1
  , scTEST   = 2
};


// Messages
int k_msgSend   (int iConnectionID, const void * pSndMsg, int iSndSize, void * pRcvMsg, int iRcvSize);
int k_msgReceive(int iChannelID, void * pRcvMsg, int iRcvSize);
int k_msgReply  (int iReceiveID, int iStatus, const void * pReplyMsg, int iReplySize);
#define msgSend     k_msgSend
#define msgReceive  k_msgReceive
#define msgReply    k_msgReply


#endif
