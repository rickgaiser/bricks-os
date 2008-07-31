include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


OBJ             =kernel
OBJS            =\
                bricks \
                debug \
                driverMountPoint \
                externalMemory \
                fileDriver \
                fs \
                genwait \
                interrupt \
                interruptManager \
                pthread \
                ringBuffer \
                settings \
                srr \
                srrChannel \
                srrConnection \
                srrNameServer \
                srrNodes \
                task \
                timeValue \
                unistd \
                video \


OBJS-$(CONFIG_FRAMEBUFFER)         += videoManager 2dRenderer
OBJS-$(CONFIG_BUILTIN_MM)          += memoryManager

OBJS-$(CONFIG_FILESYSTEM)          += fileSystem
OBJS-$(CONFIG_FS_FAT)              += fatDriver
OBJS-$(CONFIG_PD_IBM)              += ibmPartitionDriver


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
