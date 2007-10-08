#ifndef PS2_VIDEODEVICE_H
#define PS2_VIDEODEVICE_H


#include "kernel/videoManager.h"
#include "kernel/fs.h"


#define TEXT_WIDTH 45
#define TEXT_HEIGHT 17


//---------------------------------------------------------------------------
typedef enum
{
   PAL_256_256_32=0
  ,PAL_320_256_32
  ,PAL_384_256_32
  ,PAL_512_256_32
  ,PAL_640_256_32

  ,NTSC_256_224_32
  ,NTSC_320_224_32
  ,NTSC_384_224_32
  ,NTSC_512_224_32
  ,NTSC_640_224_32
} g2_video_mode;

//---------------------------------------------------------------------------
class CPS2Surface
 : public CSurface
 , public IFileIO
{
public:
  CPS2Surface();
  virtual ~CPS2Surface();

  // Inherited from IFileIO
  virtual ssize_t  write(const void * buffer, size_t size, loff_t * = 0);

  virtual void     setPixel(int x, int y);
  virtual void     fill();
  virtual void     fillRect(int x, int y, unsigned int width, unsigned int height);
  virtual void     drawLine(int x1, int y1, int x2, int y2);
  virtual void     drawRect(int x, int y, unsigned int width, unsigned int height);

  // Swap back buffer to front buffer, only if back buffer exists
  virtual void     swap(bool sync = false);

  // Wait for Horizontal Synchronization
  virtual void     waitHSync();
  // Wait for Vertical Synchronization
  virtual void     waitVSync();

private:
  void setMode(g2_video_mode mode);

  void g2_put_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t *data);

  void g2_set_viewport(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  void g2_get_viewport(uint16_t *x0, uint16_t *y0, uint16_t *x1, uint16_t *y1);

  void g2_set_visible_frame(uint8_t frame);
  void g2_set_active_frame(uint8_t frame);
  uint8_t g2_get_visible_frame(void);
  uint8_t g2_get_active_frame(void);

  void g2_wait_vsync(void);
  void g2_wait_hsync(void);

  void g2_set_font(uint32_t *data, uint16_t w, uint16_t h, uint16_t *tc);
  void g2_out_text(uint16_t x, uint16_t y, char *str);
  void g2_set_font_spacing(uint16_t s);
  uint16_t g2_get_font_spacing(void);
  void g2_set_font_mag(uint16_t m);
  uint16_t g2_get_font_mag(void);
  void g2_set_font_color(uint8_t red, uint8_t green, uint8_t blue);

  unsigned int iCurrentX_;
  unsigned int iCurrentY_;

  char pBuffer_[TEXT_HEIGHT][TEXT_WIDTH];
};

//---------------------------------------------------------------------------
class CPS2VideoDevice
 : public CAVideoDevice
{
public:
  CPS2VideoDevice();
  virtual ~CPS2VideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount);
  virtual void getMode(SVideoMode ** mode);
  virtual void setMode(const SVideoMode * mode);

  virtual void getSurface(CSurface ** surface, ESurfaceType type, bool bDouble);

private:
  const SVideoMode * pCurrentMode_;
};


#endif
