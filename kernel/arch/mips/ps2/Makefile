include $(BRICKS_ROOT)/kernel/arch/$(ARCH)/config.$(TARGET)


LIB             =arch
OBJS            =\
                arch \
                ps2Video \
                task \


OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
