#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H


#include "asm/arch/config.h"
#include "asm/syscall.h"


//---------------------------------------------------------------------------
#define declareSysCallKernel0(type,name)                                                         type k_##name();
#define declareSysCallKernel1(type,name,type1,arg1)                                              type k_##name(type1 arg1);
#define declareSysCallKernel2(type,name,type1,arg1,type2,arg2)                                   type k_##name(type1 arg1, type2 arg2);
#define declareSysCallKernel3(type,name,type1,arg1,type2,arg2,type3,arg3)                        type k_##name(type1 arg1, type2 arg2, type3 arg3);
#define declareSysCallKernel4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)             type k_##name(type1 arg1, type2 arg2, type3 arg3, type4 arg4);
#define declareSysCallKernel5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)  type k_##name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5);

//---------------------------------------------------------------------------
#ifdef CONFIG_DIRECT_ACCESS_KERNEL_FUNC
  #define declareSysCallUser0(type,name)                                                         inline type name()                                                          {return k_##name();};
  #define declareSysCallUser1(type,name,type1,arg1)                                              inline type name(type1 arg1)                                                {return k_##name(arg1);};
  #define declareSysCallUser2(type,name,type1,arg1,type2,arg2)                                   inline type name(type1 arg1, type2 arg2)                                    {return k_##name(arg1, arg2);};
  #define declareSysCallUser3(type,name,type1,arg1,type2,arg2,type3,arg3)                        inline type name(type1 arg1, type2 arg2, type3 arg3)                        {return k_##name(arg1, arg2, arg3);};
  #define declareSysCallUser4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)             inline type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4)            {return k_##name(arg1, arg2, arg3, arg4);};
  #define declareSysCallUser5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)  inline type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5){return k_##name(arg1, arg2, arg3, arg4, arg5);};
#else
  #define declareSysCallUser0(type,name)                                                         type name();
  #define declareSysCallUser1(type,name,type1,arg1)                                              type name(type1 arg1);
  #define declareSysCallUser2(type,name,type1,arg1,type2,arg2)                                   type name(type1 arg1, type2 arg2);
  #define declareSysCallUser3(type,name,type1,arg1,type2,arg2,type3,arg3)                        type name(type1 arg1, type2 arg2, type3 arg3);
  #define declareSysCallUser4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)             type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4);
  #define declareSysCallUser5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)  type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5);
#endif

//---------------------------------------------------------------------------
// System Call Function numbers
#define __NR_channelCreate           1
#define __NR_channelDestroy          2
#define __NR_channelConnectAttach    3
#define __NR_channelConnectDetach    4
#define __NR_pthread_create          5
#define __NR_pthread_exit            6
#define __NR_pthread_cond_init       7
#define __NR_pthread_cond_destroy    8
#define __NR_pthread_cond_wait       9
#define __NR_pthread_cond_timedwait 10
#define __NR_pthread_cond_signal    11
#define __NR_pthread_cond_broadcast 12
#define __NR_pthread_mutex_init     13
#define __NR_pthread_mutex_destroy  14
#define __NR_pthread_mutex_lock     15
#define __NR_pthread_mutex_trylock  16
#define __NR_pthread_mutex_unlock   17

//---------------------------------------------------------------------------
#define sysCallUser0(type,name) \
type name() \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), kfCHANNEL_CREATE}, 0}; \
  msg.hdr.iHeaderSize = sizeof(SKernelMessageHeader); \
  msg.hdr.iVersion    = INTERFACE_VERSION(1,0); \
  msg.hdr.iFunctionID = __NR_##name; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), kfCHANNEL_CREATE}, 0}; \
  msg.hdr.iHeaderSize = sizeof(SKernelMessageHeader); \
  msg.hdr.iVersion    = INTERFACE_VERSION(1,0); \
  msg.hdr.iFunctionID = __NR_##name; \
  msg.arg1 = arg1; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1, type2 arg2) \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), kfCHANNEL_CREATE}, 0}; \
  msg.hdr.iHeaderSize = sizeof(SKernelMessageHeader); \
  msg.hdr.iVersion    = INTERFACE_VERSION(1,0); \
  msg.hdr.iFunctionID = __NR_##name; \
  msg.arg1 = arg1; \
  msg.arg2 = arg2; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1, type2 arg2, type3 arg3) \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
    type3 arg3; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), kfCHANNEL_CREATE}, 0}; \
  msg.hdr.iHeaderSize = sizeof(SKernelMessageHeader); \
  msg.hdr.iVersion    = INTERFACE_VERSION(1,0); \
  msg.hdr.iFunctionID = __NR_##name; \
  msg.arg1 = arg1; \
  msg.arg2 = arg2; \
  msg.arg3 = arg3; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
    type3 arg3; \
    type4 arg4; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), kfCHANNEL_CREATE}, 0}; \
  msg.hdr.iHeaderSize = sizeof(SKernelMessageHeader); \
  msg.hdr.iVersion    = INTERFACE_VERSION(1,0); \
  msg.hdr.iFunctionID = __NR_##name; \
  msg.arg1 = arg1; \
  msg.arg2 = arg2; \
  msg.arg3 = arg3; \
  msg.arg4 = arg4; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
    type3 arg3; \
    type4 arg4; \
    type5 arg5; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), kfCHANNEL_CREATE}, 0}; \
  msg.hdr.iHeaderSize = sizeof(SKernelMessageHeader); \
  msg.hdr.iVersion    = INTERFACE_VERSION(1,0); \
  msg.hdr.iFunctionID = __NR_##name; \
  msg.arg1 = arg1; \
  msg.arg2 = arg2; \
  msg.arg3 = arg3; \
  msg.arg4 = arg4; \
  msg.arg5 = arg5; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define unwrapfunc0(name) \
case __NR_##name: \
{ \
  iRetVal = k_##name(); \
  break; \
}

//---------------------------------------------------------------------------
#define unwrapfunc1(name,data,type1,arg1) \
case __NR_##name: \
{ \
  struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
  } __attribute__ ((__packed__)); \
  SMsg * msg = (SMsg *)data; \
  iRetVal = k_##name(msg->arg1); \
  break; \
}

//---------------------------------------------------------------------------
#define unwrapfunc2(name,data,type1,arg1,type2,arg2) \
case __NR_##name: \
{ \
  struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
  } __attribute__ ((__packed__)); \
  SMsg * msg = (SMsg *)data; \
  iRetVal = k_##name(msg->arg1, msg->arg2); \
  break; \
}

//---------------------------------------------------------------------------
#define unwrapfunc3(name,data,type1,arg1,type2,arg2,type3,arg3) \
case __NR_##name: \
{ \
  struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
    type3 arg3; \
  } __attribute__ ((__packed__)); \
  SMsg * msg = (SMsg *)data; \
  iRetVal = k_##name(msg->arg1, msg->arg2, msg->arg3); \
  break; \
}

//---------------------------------------------------------------------------
#define unwrapfunc4(name,data,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
case __NR_##name: \
{ \
  struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
    type3 arg3; \
    type4 arg4; \
  } __attribute__ ((__packed__)); \
  SMsg * msg = (SMsg *)data; \
  iRetVal = k_##name(msg->arg1, msg->arg2, msg->arg3, msg->arg4); \
  break; \
}

//---------------------------------------------------------------------------
#define unwrapfunc5(name,data,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
case __NR_##name: \
{ \
  struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
    type3 arg3; \
    type4 arg4; \
    type5 arg5; \
  } __attribute__ ((__packed__)); \
  SMsg * msg = (SMsg *)data; \
  iRetVal = k_##name(msg->arg1, msg->arg2, msg->arg3, msg->arg4, msg->arg5); \
  break; \
}



#endif
