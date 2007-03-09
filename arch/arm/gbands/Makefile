ifeq ($(TARGET),gba)
DIRS            =gba
LIBS            +=archgba
endif
ifeq ($(TARGET),nds7)
DIRS            =nds nds7
LIBS            +=archnds archnds7
endif
ifeq ($(TARGET),nds9)
DIRS            =nds nds9
LIBS            +=archnds archnds9
endif

LIB             =arch
OBJS            =\
                arch \
                irq \
                isr \
                timer \


ifeq ($(TARGET),gba)
OBJS            += keyboard video videoDevice
endif
ifeq ($(TARGET),nds7)
OBJS            += keyboard
endif
ifeq ($(TARGET),nds9)
OBJS            += video videoDevice
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
