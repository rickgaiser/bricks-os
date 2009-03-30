include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


VCL_INCL_PATH   =\
                -Ivu/vcl_include \

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
                irq \
                isr \
                task \

OBJS-$(CONFIG_USE_VU0) +=\
                vu0 \

OBJS-$(CONFIG_USE_VU1) +=\
                vu/test_01 \
                vu/test_02 \
                vu/test_03 \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FRAMEBUFFER)         += videoDevice
OBJS-$(CONFIG_GL)                  += glesContext


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
