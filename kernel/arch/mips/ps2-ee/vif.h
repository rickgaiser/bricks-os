#ifndef PS2_VIF_H
#define PS2_VIF_H


#include "inttypes.h"
#include "dma.h"


// VIF Commands
#define VIF_CMD_INTERRUPT     0x80
#define VIF_CMD_NOP           0x00
#define VIF_CMD_STCYCL        0x01
#define VIF_CMD_OFFSET        0x02
#define VIF_CMD_BASE          0x03
#define VIF_CMD_ITOP          0x04
#define VIF_CMD_STMOD         0x05
#define VIF_CMD_MSKPATH3      0x06
#define VIF_CMD_MARK          0x07
#define VIF_CMD_FLUSHE        0x10
#define VIF_CMD_FLUSH         0x11
#define VIF_CMD_FLUSHA        0x13
#define VIF_CMD_MSCAL         0x14
#define VIF_CMD_MSCNT         0x17
#define VIF_CMD_MSCALF        0x15
#define VIF_CMD_STMASK        0x20
#define VIF_CMD_STROW         0x30
#define VIF_CMD_STCOL         0x31
#define VIF_CMD_MPG           0x4a
#define VIF_CMD_DIRECT        0x50
#define VIF_CMD_DIRECTHL      0x51
// UNPACK Part 1
#define VIF_CMD_UNPACK        0x60
// UNPACK Part 2
#define VIF_CMD_UNPACK_SIGNED (0<<4)
#define VIF_CMD_UNPACK_UNSIGNED (1<<4)
// UNPACK Part 3
#define VIF_CMD_UNPACK_V1_32  (0<<2|0)
#define VIF_CMD_UNPACK_V1_16  (0<<2|1)
#define VIF_CMD_UNPACK_V1_8   (0<<2|2)
#define VIF_CMD_UNPACK_V2_32  (1<<2|0)
#define VIF_CMD_UNPACK_V2_16  (1<<2|1)
#define VIF_CMD_UNPACK_V2_8   (1<<2|2)
#define VIF_CMD_UNPACK_V3_32  (2<<2|0)
#define VIF_CMD_UNPACK_V3_16  (2<<2|1)
#define VIF_CMD_UNPACK_V3_8   (2<<2|2)
#define VIF_CMD_UNPACK_V4_32  (3<<2|0)
#define VIF_CMD_UNPACK_V4_16  (3<<2|1)
#define VIF_CMD_UNPACK_V4_8   (3<<2|2)
#define VIF_CMD_UNPACK_V4_5   (3<<2|3)
// UNPACK Example:
//   Unpack 4 x int32_t
//   (VIF_CMD_UNPACK | VIF_CMD_UNPACK_SIGNED | VIF_CMD_UNPACK_V4_32)

// NUM:
//  - Data: number of 128bit fields
//  - Code: number of  64bit fields
#define VIF_CODE(imm, num, cmd) \
  (((uint32_t)(imm) <<  0) | \
   ((uint32_t)(num) << 16) | \
   ((uint32_t)(cmd) << 24))


class CVIFPacket
 : public CSCDMAPacket
{
public:
  CVIFPacket(uint32_t qwSize, EDMAChannel channelId);
  CVIFPacket(void * buffer, uint32_t qwSize, EDMAChannel channelId);
  ~CVIFPacket();

  // Reset packet
  inline void reset();

  // Add VIF code
  inline void vifAddCommand(uint8_t command, uint8_t num, uint8_t immediate);

  // Send code
  void vifAddSendCode(const void * code, uint32_t size);

  // Pad to 128bit (for faster operation)
  inline void pad128();
};


#include "vif.inl"


#endif // PS2_VIF_H
