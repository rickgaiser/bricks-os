// Based on Deku's gba-mod tutorial: http://deku.gbadev.org/
#ifndef ARCH_GBASOUND_H
#define ARCH_GBASOUND_H


#include "kernel/interrupt.h"
#include "asm/arch/memory.h"


#define SOUND_CHANNEL_COUNT  4


// -----------------------------------------------------------------------------
struct SSoundChannel
{
  const int8_t * data;
  uint32_t pos;
  uint32_t inc;
  uint32_t vol;
  uint32_t length;
  uint32_t loopLength;
};

// -----------------------------------------------------------------------------
struct SSoundBuffer
{
  int8_t   * mixBufferBase;  // Pointer to entire mix buffer (2 buffers)
  int8_t   * curMixBuffer;   // Pointer to current mix buffer
  uint32_t   mixBufferSize;  // Size of 1 mix buffer
  uint16_t   mixFreq;        // Output frequency
  uint8_t    activeBuffer;   // The current active buffer (0 or 1)
};

// -----------------------------------------------------------------------------
class CGBASound
 : public IInterruptServiceRoutine
{
public:
  CGBASound();
  virtual ~CGBASound();

  int init();

  void setChannel(uint8_t nr, SSoundChannel * data);
  void mix(uint32_t samplesToMix) FAST_CODE;

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq) INTERRUPT_CODE;

private:
  // Input channels
  SSoundChannel * channel_[SOUND_CHANNEL_COUNT];

  // Hardware output channels
  SSoundBuffer buffera_;
  //SSoundBuffer bufferb_;
  int8_t bufferaData_[736*2];
  //int8_t bufferbData_[736*2];
};


#endif
