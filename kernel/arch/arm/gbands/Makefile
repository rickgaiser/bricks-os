include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =arch
OBJS            =\
                arch \
                irq \
                isr \
                task \
                timer \


OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice

ifeq ($(TARGET),nds9)
OBJS-$(CONFIG_GL)                  += glesContextNDS matrixNDS texturesNDS
else
OBJS-$(CONFIG_GL)                  += glesContext
endif

OBJS-$(CONFIG_DEBUGGING)           += font8x8 debugScreen
OBJS-$(CONFIG_GBA_KEYBOARD)        += keyboard
OBJS-$(CONFIG_GBA_SERIAL)          += serial
OBJS-$(CONFIG_GBA_SOUND)           += gbaSound modPlayer SndData_s SndData_c
OBJS-$(CONFIG_GBA_MULTIBOOT)       += gbaMultiboot
OBJS-$(CONFIG_NDS_IPC)             += dsIPC
OBJS-$(CONFIG_FILESYSTEM)          += superCardDriver


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
