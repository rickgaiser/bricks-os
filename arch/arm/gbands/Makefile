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
                isr \
                arch \
                irq \
                timer \

ifeq ($(TARGET),gba)
OBJS            += video keyboard
endif
ifeq ($(TARGET),nds7)
OBJS            += keyboard
endif
ifeq ($(TARGET),nds9)
OBJS            += video
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
