#ifndef ARM_SYSCALL_H
#define ARM_SYSCALL_H


#include "asm/arch/registers.h"


//---------------------------------------------------------------------------
#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
  return REG_MSGRECEIVE(arg1, arg2, arg3); \
}

//---------------------------------------------------------------------------
#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name(type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
  return REG_MSGREPLY(arg1, arg2, arg3, arg4); \
}

//---------------------------------------------------------------------------
#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
type name(type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
  return REG_MSGSEND(arg1, arg2, arg3, arg4, arg5); \
}


#endif
