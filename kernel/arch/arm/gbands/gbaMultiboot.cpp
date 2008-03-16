#include "kernel/debug.h"
#include "asm/arch/registers.h"
#include "inttypes.h"


struct MultiBootParam
{
  uint32_t   reserved1[5];
  uint8_t    handshake_data;
  uint8_t    padding;
  uint16_t   handshake_timeout;
  uint8_t    probe_count;
  uint8_t    client_data[3];
  uint8_t    palette_data;
  uint8_t    response_bit;
  uint8_t    client_bit;
  uint8_t    reserved2;
  uint8_t  * boot_srcp;
  uint8_t  * boot_endp;
  uint8_t  * masterp;
  uint8_t  * reserved3[3];
  uint32_t   system_work2[4];
  uint8_t    sendflag;
  uint8_t    probe_target_bit;
  uint8_t    check_wait;
  uint8_t    server_type;
};

//enum MULTIBOOT_MODES { MODE32_NORMAL, MODE16_MULTI, MODE32_2MHZ};


MultiBootParam mbp;
uint8_t * pHeader = (uint8_t *)0x8000000;
uint8_t * pData   = (uint8_t *)0x80000c0;

char * bootModes[] =
{
  "NULL",
  "Joybus",
  "Normal",
  "Multiplay",
};


// -----------------------------------------------------------------------------
uint16_t
xfer(uint16_t data)
{
  REG_SIOMLT_SEND = data;   // Setup 16bit value to send
  REG_SIOCNT = 0x2083;      // Init 16bit Multiplayer transfer

  while(REG_SIOCNT & 0x80); // Loop until transfer has completed

  return REG_SIOMULTI1;     // Return 16bit value that was received
}

// -----------------------------------------------------------------------------
void
SendMBImageToClient()
{
  REG_RCNT = 0; // non-general purpose comms

  uint16_t data;
  int iSlaveID(-1);

  // Wait for slave to enter 
  printk("Slave mode...");
  while(xfer(0x6200) != 0x0000);
  printk("ok\n");

  for(int i(0); i < 15; i++)
  {
    uint16_t data = xfer(0x6200);
    if((data & 0xfff0) == 0x7200)
    {
      iSlaveID = data & 0x000f;
      printk("Slave %d found\n", iSlaveID);
      break;
    }
    else
    {
      // Wait 1/16 sec
    }
  }
  if(iSlaveID == -1)
  {
    printk("No slaves!\n");
    return;
  }

  data = xfer(0x6100 + iSlaveID);
  if(data == (0x7200 + iSlaveID))
    printk("Slave ACK ok\n");
  else
  {
    printk("Slave ACK error\n");
    return;
  }

  // Send multiboot header
  for(int i(0); i < 0x60; i++)
    xfer((pHeader[i*2+1] << 8) + pHeader[i*2]);

  data = xfer(0x6200);
  if(data == (0x0000 + iSlaveID))
    printk("Slave ACK ok\n");
  else
  {
    printk("Slave ACK error\n");
    return;
  }

  data = xfer(0x6200 + iSlaveID);
  if(data == 0x7200 + iSlaveID)
    printk("Slave ACK ok\n");
  else
  {
    printk("Slave ACK error\n");
    return;
  }

  uint8_t palette_data(0xc1);
  uint8_t client_data(0);
  printk("Slave palette mode...");
  while(true)
  {
    data = xfer(0x6300 + palette_data);
    if((data & 0xff00) == 0x7300)
    {
      client_data = data & 0x00ff;
      break;
    }
  }
  printk("ok\n");

  uint8_t handshake_data(0x11 + client_data + 0xff + 0xff);
  data = xfer(0x6400 + handshake_data);
  if((data & 0xff00) == 0x7300)
    printk("Slave ACK ok\n");
  else
  {
    printk("Slave ACK error\n");
    return;
  }

  printk("Sending...");

  uint32_t length = (256*1024) - 0xc0;
  length = (length + 15) & ~15; // 16 byte units
  if (length < 0x1c0)
   length = 0x1c0;

  mbp.client_bit     = 2;
  mbp.probe_count    = 0xd1;
  mbp.boot_srcp      = pData;
  mbp.boot_endp      = (uint8_t *)pData + length;
  mbp.palette_data   = palette_data;
  mbp.client_data[0] = client_data;
  mbp.client_data[1] = 0xff;
  mbp.client_data[2] = 0xff;
  mbp.handshake_data = handshake_data;

  // Execute BIOS routine to transfer client binary to slave unit
  asm volatile
  (
    " ldr r0,=mbp\n"
    " mov r1,#1\n"
    " swi 0x25\n"       //NOTE!!!!! Use 0x250000 for ARM C Compiler mode.
    :  // Outputs       // 0x25 here will only work for Thumb mode.
    :  // Inputs
    : "r0","r1","r2","r8","r9","r10","r11","r12"     // Regs crushed & smushed
  );

  printk("ok\n");
}


//---------------------------------------------------------------------------
int
multiBoot()
{
  uint8_t bootMode = *((uint8_t *)0x20000c4);
  uint8_t deviceID = *((uint8_t *)0x20000c5);

  printk("Boot mode: %s\n", bootModes[bootMode & 0x03]);
  printk("Device ID: %d\n", deviceID);

  if(deviceID == 0)
    SendMBImageToClient();

  printk("Ready!\n");

  return 0;
}
