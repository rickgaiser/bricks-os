include $(BRICKS_ROOT)/kernel/include/include-$(ARCH)/include-$(TARGET)/asm/arch/config.make


OBJ             =arch
OBJS            =\
                icxxabi \
                entry \
                apic \
                arch \
                cpuid \
                descriptor \
                drivers \
                _gpf \
                gpf \
                i8042 \
                i8237 \
                i8254 \
                i8259 \
                isrWrapper \
                isr \
                keyboard \
                pmm \
                vmm \
                mouse \
                pci \
                serial \
                support \
                task \
                mp \
                mpBoot \


OBJS-$(CONFIG_DEBUGGING)           += debugScreen
OBJS-$(CONFIG_FILESYSTEM)          += ata
OBJS-$(CONFIG_FRAMEBUFFER)         += vesa
OBJS-$(CONFIG_AUDIO_SB16)          += sb16


include $(BRICKS_ROOT)/include/makeinclude/rules.kernel
