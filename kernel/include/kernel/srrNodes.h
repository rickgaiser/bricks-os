#ifndef KERNEL_SRRNODES_H
#define KERNEL_SRRNODES_H


#include "kernel/syscall.h"
#include "kernel/srrChannel.h"
#include "sys/types.h"
#include "inttypes.h"


#define MAX_NODES         10
#define NODEID_TO_NODEIDX(id)  ((id)-1)
#define NODEIDX_TO_NODEID(idx) ((idx)+1)


class CGateway
{
public:
  CGateway(int iPID, int iChannelIDControl, int iChannelIDData);
  virtual ~CGateway();

  int channelConnectAttach(pid_t iProcessID, int iChannelID, int iFlags);

private:
  // Connection to the control channel
  CConnection * ctrl_;

  int iPID_;
  int iChannelIDControl_;
  int iChannelIDData_;
};


extern CGateway * nodes_[MAX_NODES];


#endif
