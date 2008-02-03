include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =arch
OBJS            =\
                entry \
                apic \
                arch \
                aspace \
                cpuid \
                descriptor \
                irq \
                isrWrapper \
                keyboard \
                mmap \
                support \
                task \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FILESYSTEM)          += ata


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
