include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =arch
OBJS            =\
                arch \
                irq \
                isr \
                task \
                timer \


OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice
OBJS-$(CONFIG_GL)                  += glesContext
OBJS-$(CONFIG_GBA_CONSOLE)         += font8x8 video
OBJS-$(CONFIG_GBA_KEYBOARD)        += keyboard
OBJS-$(CONFIG_GBA_SERIAL)          += serial
OBJS-$(CONFIG_NDS_IPC)             += dsIPC


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
