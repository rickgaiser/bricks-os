include $(BRICKS_ROOT)/kernel/arch/$(ARCH)/config.$(TARGET)


INCL_PATH       =\
                $(BRICKS_ROOT)/include \
                $(BRICKS_ROOT)/kernel/include \

LIB             =srr
OBJS            =\
                srrTest \


ifneq ($(DIRECT_ACCESS_CHANNEL),y)
OBJS            += srrChannel
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.user
