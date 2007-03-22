include $(BRICKS_ROOT)/arch/$(ARCH)/config.$(TARGET)


LIB             =kernel
OBJS            =\
                bricks \
                fileio \
                interrupt \
                interruptManager \
                memoryManager \
                pthread \
                task \
                timeValue \
                video \

#OBJS-$(CONFIG_MULTITASKING)        += pthread task
OBJS-$(CONFIG_FRAMEBUFFER)         += videoManager
OBJS-$(CONFIG_GL)                  += gl
OBJS-$(CONFIG_SRR)                 += srr srrTest

DEFINITIONS-$(CONFIG_MULTITASKING) += CONFIG_MULTITASKING
DEFINITIONS-$(CONFIG_FRAMEBUFFER)  += CONFIG_FRAMEBUFFER
DEFINITIONS-$(CONFIG_GL)           += CONFIG_GL
DEFINITIONS-$(CONFIG_BWM)          += CONFIG_BWM
DEFINITIONS-$(CONFIG_SRR)          += CONFIG_SRR


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
