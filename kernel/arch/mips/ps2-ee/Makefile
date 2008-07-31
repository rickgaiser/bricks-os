include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


OBJ             =arch
OBJS            =\
                arch \
                bios \
                courier_new \
                fixed_tc \
                gif \
                gs \
                matrix \
                sif \
                task \
                vu \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice
OBJS-$(CONFIG_GL)                  += glesContext


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
