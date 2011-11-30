/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H


#include "asm/arch/config.h"
#include "asm/syscall.h"
#include "stdint.h"


//---------------------------------------------------------------------------
#define declareSysCallKernel1(name,type1,arg1)                                                    void k_##name(type1 arg1);

#define declareSysCallKernel0r(type,name)                                                         type k_##name();
#define declareSysCallKernel1r(type,name,type1,arg1)                                              type k_##name(type1 arg1);
#define declareSysCallKernel2r(type,name,type1,arg1,type2,arg2)                                   type k_##name(type1 arg1, type2 arg2);
#define declareSysCallKernel3r(type,name,type1,arg1,type2,arg2,type3,arg3)                        type k_##name(type1 arg1, type2 arg2, type3 arg3);
#define declareSysCallKernel4r(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)             type k_##name(type1 arg1, type2 arg2, type3 arg3, type4 arg4);
#define declareSysCallKernel5r(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)  type k_##name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5);

//---------------------------------------------------------------------------
#ifdef CONFIG_DIRECT_ACCESS_KERNEL_FUNC
  #define declareSysCallUser1(name,type1,arg1)                                                    inline void name(type1 arg1)                                                {k_##name(arg1);};

  #define declareSysCallUser0r(type,name)                                                         inline type name()                                                          {return k_##name();};
  #define declareSysCallUser1r(type,name,type1,arg1)                                              inline type name(type1 arg1)                                                {return k_##name(arg1);};
  #define declareSysCallUser2r(type,name,type1,arg1,type2,arg2)                                   inline type name(type1 arg1, type2 arg2)                                    {return k_##name(arg1, arg2);};
  #define declareSysCallUser3r(type,name,type1,arg1,type2,arg2,type3,arg3)                        inline type name(type1 arg1, type2 arg2, type3 arg3)                        {return k_##name(arg1, arg2, arg3);};
  #define declareSysCallUser4r(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)             inline type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4)            {return k_##name(arg1, arg2, arg3, arg4);};
  #define declareSysCallUser5r(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)  inline type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5){return k_##name(arg1, arg2, arg3, arg4, arg5);};
#else
  #define declareSysCallUser1(name,type1,arg1)                                                    void name(type1 arg1);

  #define declareSysCallUser0r(type,name)                                                         type name();
  #define declareSysCallUser1r(type,name,type1,arg1)                                              type name(type1 arg1);
  #define declareSysCallUser2r(type,name,type1,arg1,type2,arg2)                                   type name(type1 arg1, type2 arg2);
  #define declareSysCallUser3r(type,name,type1,arg1,type2,arg2,type3,arg3)                        type name(type1 arg1, type2 arg2, type3 arg3);
  #define declareSysCallUser4r(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4)             type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4);
  #define declareSysCallUser5r(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)  type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5);
#endif

//---------------------------------------------------------------------------
// Predefined Connections
enum EStaticConnectionID
{
    scERROR  = 0  // Error detection for connection ID == 0
  , scKERNEL = 1
};

// -----------------------------------------------------------------------------
// Header to be included in every kernel message
// size: 12 bytes
struct SKernelMessageHeader
{
  // Size of this header (so the header can be extended in the future)
  uint32_t iHeaderSize;
  // Version of message being sent (allows backward compatibility in the future)
  uint32_t iVersion;
  // ID of function to be called
  uint32_t iFunctionID;
} __attribute__ ((__packed__));

// -----------------------------------------------------------------------------
// MACRO for interface version
// Message version format: 8.24, major.minor
//   Major: interface version (interface changes MUST have a new major version)
//   Minor: interface additions (add new functionality to the interface)
// Note:
//   Try only to add functionality to the interface, try to prevent changing
//   it. If the interface has to change, try to be backward compatible.
#define INTERFACE_VERSION(major, minor) ((major<<24)+minor)

//---------------------------------------------------------------------------
// Kernel System Call Function numbers
#define __NR_channelCreate               1
#define __NR_channelDestroy              2
#define __NR_channelConnectAttach        3
#define __NR_channelConnectDetach        4
#define __NR_pthread_create              5
#define __NR_pthread_exit                6
#define __NR_pthread_join                7
#define __NR_pthread_cond_init           8
#define __NR_pthread_cond_destroy        9
#define __NR_pthread_cond_wait          10
#define __NR_pthread_cond_timedwait     11
#define __NR_pthread_cond_signal        12
#define __NR_pthread_cond_broadcast     13
#define __NR_pthread_mutex_init         14
#define __NR_pthread_mutex_destroy      15
#define __NR_pthread_mutex_lock         16
#define __NR_pthread_mutex_trylock      17
#define __NR_pthread_mutex_unlock       18
//#define __NR_brk                        19
//#define __NR_close                      20
//#define __NR__exit                      21
#define __NR_getpid                     22
//#define __NR_read                       23
//#define __NR_sbrk                       24
#define __NR_sleep                      25
#define __NR_usleep                     26
//#define __NR_write                      27
#define __NR_registerName               28
#define __NR_lookupName                 29

//---------------------------------------------------------------------------
// Mount Point System Call Function numbers
#define __NR_open                        1
#define __NR_close                       2
#define __NR_read                        3
#define __NR_write                       4

//---------------------------------------------------------------------------
#define sysCallUser1(name,type1,arg1) \
void name(type1 arg1) \
{ \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), __NR_##name}}; \
  msg.arg1 = arg1; \
  msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
}

//---------------------------------------------------------------------------
#define sysCallUser0r(type,name) \
type name() \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), __NR_##name}}; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser1r(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), __NR_##name}}; \
  msg.arg1 = arg1; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser2r(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1, type2 arg2) \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), __NR_##name}}; \
  msg.arg1 = arg1; \
  msg.arg2 = arg2; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser3r(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1, type2 arg2, type3 arg3) \
{ \
  int iRetVal; \
  static struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
    type3 arg3; \
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), __NR_##name}}; \
  msg.arg1 = arg1; \
  msg.arg2 = arg2; \
  msg.arg3 = arg3; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser4r(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
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
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), __NR_##name}}; \
  msg.arg1 = arg1; \
  msg.arg2 = arg2; \
  msg.arg3 = arg3; \
  msg.arg4 = arg4; \
  iRetVal = msgSend(scKERNEL, &msg, sizeof(SMsg), 0, 0); \
  return (type)(iRetVal); \
}

//---------------------------------------------------------------------------
#define sysCallUser5r(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
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
  } __attribute__ ((__packed__)) msg = {{sizeof(SKernelMessageHeader), INTERFACE_VERSION(1,0), __NR_##name}}; \
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
#define kunwrapfunc0r(name) \
  unwrapfunc0r(name,k_##name)
#define unwrapfunc0r(name,cname) \
case __NR_##name: \
{ \
  iRetVal = cname(); \
  break; \
}

//---------------------------------------------------------------------------
#define kunwrapfunc1(name,data,type1,arg1) \
  unwrapfunc1(name,k_##name,data,type1,arg1)
#define unwrapfunc1(name,cname,data,type1,arg1) \
case __NR_##name: \
{ \
  struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
  } __attribute__ ((__packed__)); \
  SMsg * msg = (SMsg *)data; \
  cname(msg->arg1); \
  break; \
}

//---------------------------------------------------------------------------
#define kunwrapfunc1r(name,data,type1,arg1) \
  unwrapfunc1r(name,k_##name,data,type1,arg1)
#define unwrapfunc1r(name,cname,data,type1,arg1) \
case __NR_##name: \
{ \
  struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
  } __attribute__ ((__packed__)); \
  SMsg * msg = (SMsg *)data; \
  iRetVal = cname(msg->arg1); \
  break; \
}

//---------------------------------------------------------------------------
#define kunwrapfunc2r(name,data,type1,arg1,type2,arg2) \
  unwrapfunc2r(name,k_##name,data,type1,arg1,type2,arg2)
#define unwrapfunc2r(name,cname,data,type1,arg1,type2,arg2) \
case __NR_##name: \
{ \
  struct SMsg \
  { \
    SKernelMessageHeader hdr; \
    type1 arg1; \
    type2 arg2; \
  } __attribute__ ((__packed__)); \
  SMsg * msg = (SMsg *)data; \
  iRetVal = cname(msg->arg1, msg->arg2); \
  break; \
}

//---------------------------------------------------------------------------
#define kunwrapfunc3r(name,data,type1,arg1,type2,arg2,type3,arg3) \
  unwrapfunc3r(name,k_##name,data,type1,arg1,type2,arg2,type3,arg3)
#define unwrapfunc3r(name,cname,data,type1,arg1,type2,arg2,type3,arg3) \
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
  iRetVal = cname(msg->arg1, msg->arg2, msg->arg3); \
  break; \
}

//---------------------------------------------------------------------------
#define kunwrapfunc4r(name,data,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
  unwrapfunc4r(name,k_##name,data,type1,arg1,type2,arg2,type3,arg3,type4,arg4)
#define unwrapfunc4r(name,cname,data,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
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
  iRetVal = cname(msg->arg1, msg->arg2, msg->arg3, msg->arg4); \
  break; \
}

//---------------------------------------------------------------------------
#define kunwrapfunc5r(name,data,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
  unwrapfunc5r(name,k_##name,data,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5)
#define unwrapfunc5r(name,cname,data,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
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
  iRetVal = cname(msg->arg1, msg->arg2, msg->arg3, msg->arg4, msg->arg5); \
  break; \
}


#endif
