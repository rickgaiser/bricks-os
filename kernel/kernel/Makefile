include $(BRICKS_ROOT)/kernel/arch/$(ARCH)/config.$(TARGET)


LIB             =kernel
OBJS            =\
                bricks \
                debug \
                interrupt \
                interruptManager \
                pthread \
                settings \
                srr \
                string \
                task \
                timeValue \
                video \


OBJS-$(CONFIG_FRAMEBUFFER)         += videoManager
OBJS-$(CONFIG_BUILTIN_MM)          += memoryManager

OBJS-$(CONFIG_FILESYSTEM)          += fileSystem
OBJS-$(CONFIG_FS_FAT)              += fatDriver
OBJS-$(CONFIG_PD_IBM)              += ibmPartitionDriver


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
