include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =bwm
OBJS            =\
                bwm \


ifeq ($(CONFIG_GL),y)
  OBJS                               += pyramid cube crate
  ifeq ($(CONFIG_FPU),y)
    OBJS                               += testGLF
  else
    OBJS                               += testGLFx
  endif
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.user
