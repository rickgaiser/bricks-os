#ifndef KERNEL_SRRCHANNEL_H
#define KERNEL_SRRCHANNEL_H


#include "sys/types.h"
#include "inttypes.h"


#ifdef __cplusplus
extern "C" {
#endif


// Channels
int k_channelCreate(unsigned iFlags);
int k_channelDestroy(int iChannelID);
int k_channelConnectAttach(uint32_t iNodeID, pid_t iProcessID, int iChannelID, int iFlags);
int k_channelConnectDetach(int iConnectionID);
// Named channels
int k_channelCreateNamed(const char * sName, unsigned iFlags);
int k_channelDestroyNamed(const char * sName);
int k_channelConnectAttachNamed(const char * sName, int iFlags);
int k_channelConnectDetachNamed(const char * sName);


#ifdef __cplusplus
}
#endif


#endif
