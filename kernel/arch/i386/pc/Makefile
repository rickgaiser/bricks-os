include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


LIB             =arch
OBJS            =\
                entry \
                apic \
                arch \
                cpuid \
                descriptor \
                drivers \
                _gpf \
                gpf \
                i8042 \
                i8254 \
                irq \
                isrWrapper \
                keyboard \
                mmap \
                mouse \
                pci \
                serial \
                support \
                task \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FILESYSTEM)          += ata
OBJS-$(CONFIG_FRAMEBUFFER)         += vesa
OBJS-$(CONFIG_MMU)                 += aspace


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
