include $(BRICKS_ROOT)/arch/$(ARCH)/config.$(TARGET)


LIB             =kernel
OBJS            =\
                bricks \
                fileio \
                interrupt \
                interruptManager \
                settings \
                task \
                timeValue \
                video \

OBJS-$(CONFIG_MULTITASKING)        += pthread # task
OBJS-$(CONFIG_FRAMEBUFFER)         += videoManager
OBJS-$(CONFIG_SRR)                 += srr srrTest
OBJS-$(CONFIG_BUILTIN_MM)          += memoryManager

OBJS-$(CONFIG_FILESYSTEM)          += fileSystem
OBJS-$(CONFIG_FS_FAT)              += fatDriver
OBJS-$(CONFIG_PD_IBM)              += ibmPartitionDriver

DEFINITIONS-$(CONFIG_MULTITASKING) += CONFIG_MULTITASKING
DEFINITIONS-$(CONFIG_FRAMEBUFFER)  += CONFIG_FRAMEBUFFER
DEFINITIONS-$(CONFIG_BWM)          += CONFIG_BWM
DEFINITIONS-$(CONFIG_SRR)          += CONFIG_SRR
DEFINITIONS-$(CONFIG_BUILTIN_MM)   += CONFIG_BUILTIN_MM

DEFINITIONS-$(CONFIG_FILESYSTEM)   += CONFIG_FILESYSTEM
DEFINITIONS-$(CONFIG_FS_FAT)       += CONFIG_FS_FAT
DEFINITIONS-$(CONFIG_PD_IBM)       += CONFIG_PD_IBM


include $(BRICKS_ROOT)/include/makeinclude/rules.common.GNU
