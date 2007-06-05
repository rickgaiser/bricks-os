#ifndef I386_SYSCALL_H
#define I386_SYSCALL_H


// Interrupt 0x30 System Calls
#define sysCall0(function)                               __asm__ ("int $0x30"::"a"(function));}
#define sysCall1(function, arg1)                         __asm__ ("int $0x30"::"a"(function),"b"(arg1))
#define sysCall2(function, arg1, arg2)                   __asm__ ("int $0x30"::"a"(function),"b"(arg1),"c"(arg2))
#define sysCall3(function, arg1, arg2, arg3)             __asm__ ("int $0x30"::"a"(function),"b"(arg1),"c"(arg2),"d"(arg3))
#define sysCall4(function, arg1, arg2, arg3, arg4)       __asm__ ("int $0x30"::"a"(function),"b"(arg1),"c"(arg2),"d"(arg3),"S"(arg4))
#define sysCall5(function, arg1, arg2, arg3, arg4, arg5) __asm__ ("int $0x30"::"a"(function),"b"(arg1),"c"(arg2),"d"(arg3),"S"(arg4),"D"(arg5))


#endif
