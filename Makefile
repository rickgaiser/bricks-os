include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


DIRS            =\
                kernel \
                libtace \
                libtwl \
                apps \

LIBS            =\
                arch \
                kernel \
                apps \
                tace \
                twl \

ELF             =Bricks


ifeq ($(CONFIG_BUILTIN_LIBC),y)
  DIRS            += libc
  LIBS            += tc
endif

ifneq ($(CONFIG_DIRECT_ACCESS_KERNEL_FUNC),y)
  DIRS            += srr
  LIBS            += srr
endif

ifeq ($(CONFIG_FRAMEBUFFER),y)
  ifeq ($(CONFIG_GL),y)
    DIRS            += gl
    LIBS            += gl
  endif
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
