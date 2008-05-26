include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =tc
OBJS            =\
                ctype \
                errno \
                fcntl \
                stdlib \
                string \


ifneq ($(CONFIG_DIRECT_ACCESS_KERNEL_FUNC),y)
  OBJS            += pthread srr unistd
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.user
