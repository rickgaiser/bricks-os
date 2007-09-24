include $(BRICKS_ROOT)/kernel/arch/$(ARCH)/config.$(TARGET)


INCL_PATH       =\
                $(BRICKS_ROOT)/include \
                $(BRICKS_ROOT)/kernel/include \

LIB             =srr
OBJS            =\
                srrTest \


ifneq ($(CONFIG_DIRECT_ACCESS_KERNEL_FUNC),y)
OBJS            += srrChannel pthread
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.user
