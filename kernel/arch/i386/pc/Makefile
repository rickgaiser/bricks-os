include $(BRICKS_ROOT)/kernel/arch/$(ARCH)/config.$(TARGET)


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
                video \


OBJS-$(CONFIG_MULTITASKING)        += task

DEFINITIONS-$(CONFIG_MULTITASKING) += CONFIG_MULTITASKING


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
