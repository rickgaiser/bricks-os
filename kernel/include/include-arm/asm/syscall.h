#ifndef ARM_SYSCALL_H
#define ARM_SYSCALL_H


// System Call Function numbers
#define SC_ERROR        0
#define SC_SRR_SEND     1
#define SC_SRR_RECEIVE  2
#define SC_SRR_REPLY    3

// SRR System Calls
#define msgSend(arg1, arg2, arg3, arg4, arg5)     k_msgSend(arg1, arg2, arg3, arg4, arg5)
#define msgReceive(arg1, arg2, arg3)              k_msgReceive(arg1, arg2, arg3)
#define msgReply(arg1, arg2, arg3, arg4)          k_msgReply(arg1, arg2, arg3, arg4)


#endif
