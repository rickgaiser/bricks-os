include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


OBJ             =arch
OBJS            =\
                crt0 \
                arch \
                support \
                task \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
