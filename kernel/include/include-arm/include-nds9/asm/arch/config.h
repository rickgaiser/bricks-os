#ifndef CONFIG_H
#define CONFIG_H


// Kernel interface
#define CONFIG_DIRECT_ACCESS_KERNEL
#define CONFIG_DIRECT_ACCESS_KERNEL_FUNC

// File System
#undef  CONFIG_FILESYSTEM
#undef  CONFIG_FS_FAT
#undef  CONFIG_PD_IBM

// Video configuration
#undef  CONFIG_FRAMEBUFFER
#undef  CONFIG_GL

// Library configuration
#define CONFIG_BUILTIN_MM

// GBA/NDS specific configuration
#define CONFIG_GBA_CONSOLE
#undef  CONFIG_GBA_KEYBOARD

// NDS specific configuration
#define CONFIG_NDS_IPC


#endif
