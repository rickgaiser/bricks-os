include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


ifeq ($(CONFIG_FRAMEBUFFER),y)
  DIRS = testglut
else
  DIRS = halt
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
