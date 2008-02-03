include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =arch
OBJS            =\
                arch \
                task \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
