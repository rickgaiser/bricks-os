include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


DIRS            =\
                kernel \
                libtace \
                srr \

LIBS            =\
                arch \
                kernel \
                tace \
                srr \

ELF             =Bricks


ifeq ($(CONFIG_FRAMEBUFFER),y)
DIRS            += bwm
LIBS            += bwm
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
