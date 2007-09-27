include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =arch
OBJS            =\
                arch \
                ps2Video \
                task \


OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
