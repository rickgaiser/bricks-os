include $(BRICKS_ROOT)/kernel/arch/$(ARCH)/config.$(TARGET)


LIB             =arch
OBJS            =\
                arch \
                ps2Video \
                support \

OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice

DEFINITIONS-$(CONFIG_MULTITASKING) += CONFIG_MULTITASKING
DEFINITIONS-$(CONFIG_FRAMEBUFFER)  += CONFIG_FRAMEBUFFER


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
