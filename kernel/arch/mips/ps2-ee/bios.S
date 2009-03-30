.text
.p2align 3


#include "asm/arch/config.h"


// -----------
// From PS2SDK
// -----------
#ifdef CONFIG_KERNEL_MODE
#define SYSCALL(name, number) \
  .set push;            \
  .set noreorder;       \
  .text;                \
  .align 4;             \
  .globl name;          \
  .type name,@function; \
  .ent name,0;          \
name:                   \
  j  __syscall;         \
  li $3, number;        \
  nop;                  \
  .end  name;           \
  .size name,.-name;    \
  .set  pop;
#else
#define SYSCALL(name, number) \
  .set push;            \
  .set noreorder;       \
  .text;                \
  .align 4;             \
  .globl name;          \
  .type name,@function; \
  .ent name,0;          \
name:                   \
  li $3, number;        \
  syscall;              \
  jr $31;               \
  nop;                  \
  .end name;            \
  .size name,.-name;    \
  .set pop;
#endif


SYSCALL(ResetEE,                  0x01)
SYSCALL(SetGsCrt,                 0x02)
SYSCALL(Exit,                     0x04)
SYSCALL(LoadExecPS2,              0x06)
SYSCALL(ExecPS2,                  0x07)
SYSCALL(RFU009,                   0x09)
SYSCALL(AddSbusIntcHandler,       0x0a)
SYSCALL(RemoveSbusIntcHandler,    0x0b)
SYSCALL(Interrupt2Iop,            0x0c)
SYSCALL(SetVTLBRefillHandler,     0x0d)
SYSCALL(SetVCommonHandler,        0x0e)
SYSCALL(SetVInterruptHandler,     0x0f)
SYSCALL(AddIntcHandler,           0x10)
SYSCALL(AddIntcHandler2,          0x10)
SYSCALL(RemoveIntcHandler,        0x11)
SYSCALL(AddDmacHandler,           0x12)
SYSCALL(RemoveDmacHandler,        0x13)
SYSCALL(_EnableIntc,              0x14)
SYSCALL(_DisableIntc,             0x15)
SYSCALL(_EnableDmac,              0x16)
SYSCALL(_DisableDmac,             0x17)
SYSCALL(SetAlarm,                 0x18)
SYSCALL(ReleaseAlarm,             0x19)
SYSCALL(_iEnableIntc,            -0x1a)
SYSCALL(_iDisableIntc,           -0x1b)
SYSCALL(_iEnableDmac,            -0x1c)
SYSCALL(_iDisableDmac,           -0x1d)
SYSCALL(iSetAlarm,               -0x1e)
SYSCALL(iReleaseAlarm,           -0x1f)
SYSCALL(CreateThread,             0x20)
SYSCALL(DeleteThread,             0x21)
SYSCALL(StartThread,              0x22)
SYSCALL(ExitThread,               0x23)
SYSCALL(ExitDeleteThread,         0x24)
SYSCALL(TerminateThread,          0x25)
SYSCALL(iTerminateThread,        -0x26)
SYSCALL(DisableDispatchThread,    0x27)
SYSCALL(EnableDispatchThread,     0x28)
SYSCALL(ChangeThreadPriority,     0x29)
SYSCALL(iChangeThreadPriority,   -0x2a)
SYSCALL(RotateThreadReadyQueue,   0x2b)
SYSCALL(iRotateThreadReadyQueue, -0x2c)
SYSCALL(ReleaseWaitThread,        0x2d)
SYSCALL(iReleaseWaitThread,      -0x2e)
SYSCALL(GetThreadId,              0x2f)
SYSCALL(ReferThreadStatus,        0x30)
SYSCALL(iReferThreadStatus,      -0x31)
SYSCALL(SleepThread,              0x32)
SYSCALL(WakeupThread,             0x33)
SYSCALL(iWakeupThread,           -0x34)
SYSCALL(CancelWakeupThread,       0x35)
SYSCALL(iCancelWakeupThread,     -0x36)
SYSCALL(SuspendThread,            0x37)
SYSCALL(iSuspendThread,          -0x38)
SYSCALL(ResumeThread,             0x39)
SYSCALL(iResumeThread,           -0x3a)
SYSCALL(RFU059,                   0x3b)
SYSCALL(RFU060,                   0x3c)
SYSCALL(SetupThread,              0x3c)
SYSCALL(RFU061,                   0x3d)
SYSCALL(SetupHeap,                0x3d)
SYSCALL(EndOfHeap,                0x3e)
SYSCALL(CreateSema,               0x40)
SYSCALL(DeleteSema,               0x41)
SYSCALL(SignalSema,               0x42)
SYSCALL(iSignalSema,             -0x43)
SYSCALL(WaitSema,                 0x44)
SYSCALL(PollSema,                 0x45)
SYSCALL(iPollSema,               -0x46)
SYSCALL(ReferSemaStatus,          0x47)
SYSCALL(iReferSemaStatus,        -0x48)
SYSCALL(iDeleteSema,             -0x49)
SYSCALL(SetOsdConfigParam,        0x4a)
SYSCALL(GetOsdConfigParam,        0x4b)
SYSCALL(GetGsHParam,              0x4c)
SYSCALL(GetGsVParam,              0x4d)
SYSCALL(SetGsHParam,              0x4e)
SYSCALL(SetGsVParam,              0x4f)
SYSCALL(CreateEventFlag,          0x50)
SYSCALL(DeleteEventFlag,          0x51)
SYSCALL(SetEventFlag,             0x52)
SYSCALL(iSetEventFlag,            0x53)
SYSCALL(EnableIntcHandler,        0x5c)
SYSCALL(iEnableIntcHandler,      -0x5c)
SYSCALL(DisableIntcHandler,       0x5d)
SYSCALL(iDisableIntcHandler,     -0x5d)
SYSCALL(EnableDmacHandler,        0x5e)
SYSCALL(iEnableDmacHandler,      -0x5e)
SYSCALL(DisableDmacHandler,       0x5f)
SYSCALL(iDisableDmacHandler,     -0x5f)
SYSCALL(KSeg0,                    0x60)
SYSCALL(EnableCache,              0x61)
SYSCALL(DisableCache,             0x62)
SYSCALL(GetCop0,                  0x63)
SYSCALL(FlushCache,               0x64)
SYSCALL(CpuConfig,                0x66)
SYSCALL(iGetCop0,                -0x67)
SYSCALL(iFlushCache,             -0x68)
SYSCALL(RFU105,                   0x69)
SYSCALL(iCpuConfig,              -0x6a)
SYSCALL(SifStopDma,               0x6b)
SYSCALL(SetCPUTimerHandler,       0x6c)
SYSCALL(SetCPUTimer,              0x6d)
SYSCALL(SetOsdConfigParam2,       0x6e)
SYSCALL(GetOsdConfigParam2,       0x6f)
SYSCALL(GsGetIMR,                 0x70)
SYSCALL(iGsGetIMR,               -0x70)
SYSCALL(GsPutIMR,                 0x71)
SYSCALL(iGsPutIMR,               -0x71)
SYSCALL(SetPgifHandler,           0x72)
SYSCALL(SetVSyncFlag,             0x73)
SYSCALL(SetSyscall,               0x74)
SYSCALL(SifDmaStat,               0x76)
SYSCALL(iSifDmaStat,             -0x76)
SYSCALL(SifSetDma,                0x77)
SYSCALL(iSifSetDma,              -0x77)
SYSCALL(SifSetDChain,             0x78)
SYSCALL(iSifSetDChain,           -0x78)
SYSCALL(SifSetReg,                0x79)
SYSCALL(SifGetReg,                0x7a)
SYSCALL(ExecOSD,                  0x7b)
SYSCALL(Deci2Call,                0x7c)
SYSCALL(PSMode,                   0x7d)
SYSCALL(MachineType,              0x7e)
SYSCALL(GetMemorySize,            0x7f)
