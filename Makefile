include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


DIRS            =\
                kernel \
                libtace \
                apps \

LIBS            =\
                arch \
                kernel \
                apps \
                tace \

ELF             =Bricks


ifeq ($(TARGET),gba)
ELF             +=Bricks_mb
endif

ifeq ($(CONFIG_BUILTIN_LIBC),y)
  DIRS            += libc
  LIBS            += tc
endif

ifneq ($(CONFIG_DIRECT_ACCESS_KERNEL_FUNC),y)
  DIRS            += srr
  LIBS            += srr
endif

ifeq ($(CONFIG_FRAMEBUFFER),y)
  DIRS            += libtwl
  LIBS            += twl
endif

ifeq ($(CONFIG_GL),y)
  DIRS            += gl
  LIBS            += gl
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
