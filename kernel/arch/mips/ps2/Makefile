include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =arch
OBJS            =\
                arch \
                bios \
                courier_new \
                dma_asm \
                fixed_tc \
                gif \
                gs \
                task \

#                irq \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice
OBJS-$(CONFIG_GL)                  += glesContext


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
