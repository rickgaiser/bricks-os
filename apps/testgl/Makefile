include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


OBJ             =apps
OBJS            =\
                testGL \
                teapot \
                pyramid \
                cube \
                crate \


ifeq ($(CONFIG_FPU),y)
  OBJS                               += testGLF
else
  OBJS                               += testGLFx
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.user
