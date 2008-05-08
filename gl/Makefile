include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =gl
OBJS            =\
                context \
                gl \
                glMatrix \
                glu \
                textures \


ifeq ($(CONFIG_FPU),y)
  OBJS                               += softGLF
else
  OBJS                               += softGLFx
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.user
