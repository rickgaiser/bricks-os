include $(BRICKS_ROOT)/arch/$(ARCH)/config.$(TARGET)


DIRS            =\
                arch \
                kernel \
                stl \

LIBS            =\
                arch \
                kernel \
                stl \

ELF             =Bricks

ifeq ($(CONFIG_BWM),y)
DIRS            += bwm
LIBS            += bwm
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
