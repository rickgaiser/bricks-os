include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


OBJ             =arch
OBJS            =\
                arch \
                bios \
                cache \
                courier_new \
                dma \
                drivers \
                fixed_tc \
                gif \
                vif \
                gs \
                irq \
                isr \
                matrix \
                sif \
                task \
                vu \
                vu0 \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice
OBJS-$(CONFIG_GL)                  += glesContext


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
