include $(BRICKS_ROOT)/arch/$(ARCH)/config.$(TARGET)


LIB             =arch
OBJS            =\
                arch \
                dsCart \
                irq \
                isr \
                superCardDriver \
                support \


OBJS-$(CONFIG_MULTITASKING)        += timer
OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice
OBJS-$(CONFIG_GBA_CONSOLE)         += font8x8 video
OBJS-$(CONFIG_GBA_KEYBOARD)        += keyboard
OBJS-$(CONFIG_GBA_SERIAL)          += serial

DEFINITIONS-$(CONFIG_MULTITASKING) += CONFIG_MULTITASKING
DEFINITIONS-$(CONFIG_FRAMEBUFFER)  += CONFIG_FRAMEBUFFER
DEFINITIONS-$(CONFIG_GBA_CONSOLE)  += CONFIG_GBA_CONSOLE
DEFINITIONS-$(CONFIG_GBA_KEYBOARD) += CONFIG_GBA_KEYBOARD
DEFINITIONS-$(CONFIG_GBA_SERIAL)   += CONFIG_GBA_SERIAL


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
