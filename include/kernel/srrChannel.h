#ifndef USER_SRRCHANNEL_H
#define USER_SRRCHANNEL_H


#ifdef DIRECT_ACCESS_CHANNEL
  #include "kernel/srrChannel_k.h"

  // Channels
  #define channelCreate              k_channelCreate
  #define channelDestroy             k_channelDestroy
  #define channelConnectAttach       k_channelConnectAttach
  #define channelConnectDetach       k_channelConnectDetach
  // Named channels
  #define channelCreateNamed         k_channelCreateNamed
  #define channelDestroyNamed        k_channelDestroyNamed
  #define channelConnectAttachNamed  k_channelConnectAttachNamed
  #define channelConnectDetachNamed  k_channelConnectDetachNamed
#else
  #include "sys/types.h"
  #include "inttypes.h"

  #ifdef __cplusplus
  extern "C" {
  #endif

  // Channels
  int channelCreate(unsigned iFlags);
  int channelDestroy(int iChannelID);
  int channelConnectAttach(uint32_t iNodeID, pid_t iProcessID, int iChannelID, int iFlags);
  int channelConnectDetach(int iConnectionID);
  // Named channels
  int channelCreateNamed(const char * sName, unsigned iFlags);
  int channelDestroyNamed(const char * sName);
  int channelConnectAttachNamed(const char * sName, int iFlags);
  int channelConnectDetachNamed(const char * sName);

  #ifdef __cplusplus
  }
  #endif

#endif


#endif
