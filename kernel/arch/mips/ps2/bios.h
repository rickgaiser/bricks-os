#ifndef PS2_BIOS_H
#define PS2_BIOS_H


#include "stdint.h"


namespace bios
{


#define SIF_DMA_INT_I	0x2
#define SIF_DMA_INT_O	0x4


struct SifDmaTransfer_t
{
  void *   src;
  void *   dest;
  int      size;
  int      attr;
};

struct ee_sema_t
{
  int      count;
  int      max_count;
  int      init_count;
  int      wait_threads;
  uint32_t attr;
  uint32_t option;
};

struct ee_thread_t
{
  int      status;
  void *   func;
  void *   stack;
  int      stack_size;
  void *   gp_reg;
  int      initial_priority;
  int      current_priority;
  uint32_t attr;
  uint32_t option;
};


#ifdef __cplusplus
extern "C" {
#endif


void     ResetEE                (uint32_t init_bitfield);
void     SetGsCrt               (int16_t interlace, int16_t pal_ntsc, int16_t field);
void     Exit                   (int32_t exit_code) __attribute__((noreturn));
void     LoadExecPS2            (const char * filename, int32_t num_args, char * args[]);
int32_t  ExecPS2                (void * entry, void * gp, int num_args, char * args[]);
void     RFU009                 (uint32_t arg0, uint32_t arg1);
int32_t  AddSbusIntcHandler     (int32_t cause, void (*handler)(int call));
int32_t  RemoveSbusIntcHandler  (int32_t cause);
int32_t  Interrupt2Iop          (int32_t cause);
void     SetVTLBRefillHandler   (int32_t handler_num, void * handler_func);
void     SetVCommonHandler      (int32_t handler_num, void * handler_func);
void     SetVInterruptHandler   (int32_t handler_num, void * handler_func);
int32_t  AddIntcHandler         (int32_t cause, int32_t(*handler_func)(int32_t cause), int32_t next);
int32_t  AddIntcHandler2        (int32_t cause, int32_t(*handler_func)(int32_t cause, void * arg, void * addr), int32_t next, void * arg);
int32_t  RemoveIntcHandler      (int32_t cause, int32_t handler_id);
int32_t  AddDmacHandler         (int32_t channel, int32_t (*handler)(int32_t channel), int32_t next);
int32_t  RemoveDmacHandler      (int32_t channel, int32_t handler_id);
int32_t  _EnableIntc            (int32_t cause);
int32_t  _DisableIntc           (int32_t cause);
int32_t  _EnableDmac            (int32_t channel);
int32_t  _DisableDmac           (int32_t channel);
int32_t  SetAlarm               (uint16_t time, void (*callback)(int32_t alarm_id, uint16_t time, void * arg2), void * arg2);
int32_t  ReleaseAlarm           (int32_t alarm_id);
int32_t  _iEnableIntc           (int32_t cause);
int32_t  _iDisableIntc          (int32_t cause);
int32_t  _iEnableDmac           (int32_t channel);
int32_t  _iDisableDmac          (int32_t channel);
int32_t  iSetAlarm              (uint16_t time, void (*callback)(int32_t alarm_id, uint16_t time, void * arg2), void * arg2);
int32_t  iReleaseAlarm          (int32_t alarm_id);
int32_t  CreateThread           (ee_thread_t * thread);
int32_t  DeleteThread           (int32_t thread_id);
int32_t  StartThread            (int32_t thread_id, void * args);
void     ExitThread             (void);
void     ExitDeleteThread       (void);
int32_t  TerminateThread        (int32_t thread_id);
int32_t  iTerminateThread       (int32_t thread_id);
void     DisableDispatchThread  (void);
void     EnableDispatchThread   (void);
int32_t  ChangeThreadPriority   (int32_t thread_id, int32_t priority);
int32_t  iChangeThreadPriority  (int32_t thread_id, int32_t priority);
int32_t  RotateThreadReadyQueue (int32_t priority);
int32_t  iRotateThreadReadyQueue(int32_t priority);
int32_t  ReleaseWaitThread      (int32_t thread_id);
int32_t  iReleaseWaitThread     (int32_t thread_id);
int32_t  GetThreadId            (void);
int32_t  ReferThreadStatus      (int32_t thread_id, ee_thread_t * info);
int32_t  iReferThreadStatus     (int32_t thread_id, ee_thread_t * info);
int32_t  SleepThread            (void);
int32_t  WakeupThread           (int32_t thread_id);
int32_t  iWakeupThread          (int32_t thread_id);
int32_t  CancelWakeupThread     (int32_t thread_id);
int32_t  iCancelWakeupThread    (int32_t thread_id);
int32_t  SuspendThread          (int32_t thread_id);
int32_t  iSuspendThread         (int32_t thread_id);
int32_t  ResumeThread           (int32_t thread_id);
int32_t  iResumeThread          (int32_t thread_id);
uint8_t  RFU059                 (void);
int32_t  RFU060                 (int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) __attribute__((deprecated));
void     RFU061                 (int32_t arg0, int32_t arg1) __attribute__((deprecated));
void *   SetupThread            (void * gp, void * stack, int32_t stack_size, void * args, void * root_func);
void     SetupHeap              (void * heap_start, int32_t heap_size);
void *   EndOfHeap              (void);
int32_t  CreateSema             (ee_sema_t * sema);
int32_t  DeleteSema             (int32_t sema_id);
int32_t  SignalSema             (int32_t sema_id);
int32_t  iSignalSema            (int32_t sema_id);
int32_t  WaitSema               (int32_t sema_id);
int32_t  PollSema               (int32_t sema_id);
int32_t  iPollSema              (int32_t sema_id);
int32_t  ReferSemaStatus        (int32_t sema_id, ee_sema_t * sema);
int32_t  iReferSemaStatus       (int32_t sema_id, ee_sema_t * sema);
int32_t  iDeleteSema            (int32_t sema_id);
void     SetOsdConfigParam      (void * addr);
void     GetOsdConfigParam      (void * addr);
void     GetGsHParam            (void * addr1, void * addr2, void * addr3);
int32_t  GetGsVParam            (void);
void     SetGsHParam            (void * addr1, void * addr2, void * addr3, void * addr4);
void     SetGsVParam            (int32_t arg1);
void     EnableIntcHandler      (uint32_t arg1);
void     iEnableIntcHandler     (uint32_t arg1);
void     DisableIntcHandler     (uint32_t arg1);
void     iDisableIntcHandler    (uint32_t arg1);
void     EnableDmacHandler      (uint32_t arg1);
void     iEnableDmacHandler     (uint32_t arg1);
void     DisableDmacHandler     (uint32_t arg1);
void     iDisableDmacHandler    (uint32_t arg1);
void     KSeg0                  (int32_t arg1);
int32_t  EnableCache            (int32_t cache);
int32_t  DisableCache           (int32_t cache);
uint32_t GetCop0                (int32_t reg_id);
void     FlushCache             (int32_t operation);
uint32_t CpuConfig              (uint32_t config);
uint32_t iGetCop0               (int32_t reg_id);
void     iFlushCache            (int32_t operation);
uint32_t iCpuConfig             (uint32_t config);
void     SetCPUTimerHandler     (void (*handler)(void));
void     SetCPUTimer            (int32_t compval);
void     SetOsdConfigParam2     (void * config, int32_t size, int32_t offset);
void     GetOsdConfigParam2     (void * config, int32_t size, int32_t offset);
uint64_t GsGetIMR               (void);
uint64_t iGsGetIMR              (void);
uint64_t GsPutIMR               (uint64_t imr);
uint64_t iGsPutIMR              (uint64_t imr);
void     SetPgifHandler         (void * handler);
void     SetVSyncFlag           (uint32_t *, uint64_t *);
void     SetSyscall             (int32_t syscall_num, void * handler);
void     SifStopDma             (void);
int32_t  SifDmaStat             (uint32_t id);
int32_t  iSifDmaStat            (uint32_t id);
uint32_t SifSetDma              (SifDmaTransfer_t * sdd, int32_t len);
uint32_t iSifSetDma             (SifDmaTransfer_t * sdd, int32_t len);
void     SifSetDChain           (void);
void     iSifSetDChain          (void);
int      SifSetReg              (uint32_t register_num, int register_value);
int      SifGetReg              (uint32_t register_num);
void     ExecOSD                (int num_args, char *args[]);
int32_t  Deci2Call              (int32_t , uint32_t *);
void     PSMode                 (void);
int32_t  MachineType            (void);
int32_t  GetMemorySize          (void);
void     SifWriteBackDCache     (void * ptr, int size);
void     _SyncDCache            (void * start, void * end);
void     _InvalidDCache         (void * start, void * end);


#ifdef __cplusplus
}
#endif


};


#endif
