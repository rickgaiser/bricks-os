#ifndef PS2_SIF_H
#define PS2_SIF_H


#include "inttypes.h"


class CIOP
{
public:
  CIOP();
  ~CIOP();

  int  init();
  bool alive();
  bool sync();

private:
  uint32_t sendCmd(int cid, void * pkt, uint32_t pktsize, void * src, void * dest, int size);

  // Callback functions
  static int32_t  callBackHandler(int32_t channel);
  static void     changeAddr(void * packet, void * harg);
  static void     setSReg   (void * packet, void * harg);

  bool bInitialized_;
  int  iDMAHandlerID_;
};


#endif
