#ifndef I386_SYSCALL_H
#define I386_SYSCALL_H


// System Call Function numbers
#define SC_ERROR        0
#define SC_SRR_SEND     1
#define SC_SRR_RECEIVE  2
#define SC_SRR_REPLY    3

// Interrupt 0x30 System Calls
#define sysCall0(function)                               __asm__ ("int $0x30"::"a"(function));}
#define sysCall1(function, arg1)                         __asm__ ("int $0x30"::"a"(function),"b"(arg1))
#define sysCall2(function, arg1, arg2)                   __asm__ ("int $0x30"::"a"(function),"b"(arg1),"c"(arg2))
#define sysCall3(function, arg1, arg2, arg3)             __asm__ ("int $0x30"::"a"(function),"b"(arg1),"c"(arg2),"d"(arg3))
#define sysCall4(function, arg1, arg2, arg3, arg4)       __asm__ ("int $0x30"::"a"(function),"b"(arg1),"c"(arg2),"d"(arg3),"S"(arg4))
#define sysCall5(function, arg1, arg2, arg3, arg4, arg5) __asm__ ("int $0x30"::"a"(function),"b"(arg1),"c"(arg2),"d"(arg3),"S"(arg4),"D"(arg5))

// SRR System Calls
#define msgSend(arg1, arg2, arg3, arg4, arg5)     sysCall5(SC_SRR_SEND,    arg1, arg2, arg3, arg4, arg5)
#define msgReceive(arg1, arg2, arg3)              sysCall3(SC_SRR_RECEIVE, arg1, arg2, arg3)
#define msgReply(arg1, arg2, arg3, arg4)          sysCall4(SC_SRR_REPLY,   arg1, arg2, arg3, arg4)


#endif
