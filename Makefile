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
  ifeq ($(CONFIG_GL),y)
    DIRS            += gl
    LIBS            += gl
  endif
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
