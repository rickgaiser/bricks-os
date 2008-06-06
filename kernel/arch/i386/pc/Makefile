include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =arch
OBJS            =\
                entry \
                apic \
                arch \
                aspace \
                cpuid \
                descriptor \
                _gpf \
                gpf \
                i8042 \
                irq \
                isrWrapper \
                keyboard \
                mmap \
                mouse \
                serial \
                support \
                task \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FILESYSTEM)          += ata
OBJS-$(CONFIG_FRAMEBUFFER)         += vesa


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
