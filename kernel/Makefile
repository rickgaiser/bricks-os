include $(BRICKS_ROOT)/arch/$(ARCH)/config.$(TARGET)


LIB             =kernel
OBJS            =\
                bricks \
                fatDriver \
                fileio \
                fileSystem \
                ibmPartitionDriver \
                interrupt \
                interruptManager \
                pthread \
                task \
                timeValue \
                video \

#OBJS-$(CONFIG_MULTITASKING)        += pthread task
OBJS-$(CONFIG_FRAMEBUFFER)         += videoManager
OBJS-$(CONFIG_SRR)                 += srr srrTest
OBJS-$(CONFIG_BUILTIN_MM)          += memoryManager

DEFINITIONS-$(CONFIG_MULTITASKING) += CONFIG_MULTITASKING
DEFINITIONS-$(CONFIG_FRAMEBUFFER)  += CONFIG_FRAMEBUFFER
DEFINITIONS-$(CONFIG_BWM)          += CONFIG_BWM
DEFINITIONS-$(CONFIG_SRR)          += CONFIG_SRR
DEFINITIONS-$(CONFIG_BUILTIN_MM)   += CONFIG_BUILTIN_MM


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
