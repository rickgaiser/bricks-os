#ifndef I386_VESA_H
#define I386_VESA_H


#include "task.h"
#include "kernel/videoManager.h"
#include "kernel/2dRenderer.h"
#include "inttypes.h"


//---------------------------------------------------------------------------
struct SVBEInfo
{
  char     sig[4];
  uint8_t  ver_minor;
  uint8_t  ver_major;
  uint16_t oem_name_off;
  uint16_t oem_name_seg;
  uint32_t capabilities; // b1=1 for non-VGA board
  uint16_t mode_list_off;
  uint16_t mode_list_seg;
  uint16_t vid_mem_size; // in units of 64K
  uint8_t  reserved[492];
} __attribute__ ((__packed__));

//---------------------------------------------------------------------------
struct SVBEMode
{
  uint16_t mode_attrib; // b5=1 for non-VGA mode
  uint8_t  win_a_attrib;
  uint8_t  win_b_attrib;
  uint16_t k_per_gran;
  uint16_t win_size;
  uint16_t win_a_seg;
  uint16_t win_b_seg;
  char     reserved1[4];
  uint16_t bytes_per_row;
  char     reserved2[494];
} __attribute__ ((__packed__));

//---------------------------------------------------------------------------
class CVesaVideoDevice
 : public CAVideoDevice
{
public:
  CVesaVideoDevice();
  virtual ~CVesaVideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount);
  virtual void getCurrentMode(const SVideoMode ** mode);
  virtual void getDefaultMode(const SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, int width, int height);
  virtual void get2DRenderer(I2DRenderer ** renderer);
  virtual void get3DRenderer(I3DRenderer ** renderer);

  virtual uint32_t getFrameNr(); // Returns current frame/field number
  virtual uint32_t waitVSync(); // Returns current frame/field number, after vsync
  virtual void displaySurface(CSurface * surface);

  virtual void bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy);

private:
  bool vbeCall(uint8_t function);

  // Surface we're currently displaying
  CSurface * pSurface_;
  vuint32_t iFrameCount_; // volatile, becouse the isr updates it
  const SVideoMode * pCurrentMode_;
  CV86Thread v86thr_;
  SVBEInfo * pInfo_;
};


#endif
