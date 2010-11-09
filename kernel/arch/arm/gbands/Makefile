include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


OBJ             =arch
OBJS            =\
                arch \
                drivers \
                irq \
                isr \
                task \
                timer \


ifeq ($(TARGET),nds9)
OBJS                               += ds_mpu_setup
OBJS-$(CONFIG_GL)                  += glesContextNDS
else
OBJS-$(CONFIG_GL)                  += glesContext
endif

OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice
OBJS-$(CONFIG_DEBUGGING)           += font8x8 debugScreen
OBJS-$(CONFIG_GBA_KEYBOARD)        += keyboard
OBJS-$(CONFIG_GBA_SERIAL)          += gbaSerial
OBJS-$(CONFIG_GBA_SOUND)           += gbaSound
OBJS-$(CONFIG_NDS_IPC)             += dsIPC
OBJS-$(CONFIG_FILESYSTEM)          += superCardDriver gbfs gbfsDriver


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
