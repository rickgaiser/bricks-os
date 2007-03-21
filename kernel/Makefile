include $(BRICKS_ROOT)/arch/$(ARCH)/config.$(TARGET)


LIB             =kernel
OBJS            =\
                bricks \
                fileio \
                interrupt \
                interruptManager \
                memoryManager \
                pthread \
                srr \
                srrTest \
                task \
                timeValue \
                video \

#                taskManager \
                timerManager \

ifeq ($(CONFIG_FRAMEBUFFER),y)
OBJS            += videoManager
endif
ifeq ($(CONFIG_GL),y)
OBJS            += gl
endif


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
