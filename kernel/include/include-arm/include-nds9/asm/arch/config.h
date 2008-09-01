#ifndef CONFIG_H
#define CONFIG_H


// Debugging
#define CONFIG_DEBUGGING

// Multithreading implemented yet?
#define CONFIG_MULTI_THREADING

// Hardware capabilities
#define __LITTLE_ENDIAN__
//#define __BIG_ENDIAN__
#undef  CONFIG_FPU
#undef  CONFIG_MMU

// Kernel interface
#define CONFIG_DIRECT_ACCESS_KERNEL
#define CONFIG_DIRECT_ACCESS_KERNEL_FUNC

// File System
#define CONFIG_FILESYSTEM
#define CONFIG_FS_FAT
#define CONFIG_PD_IBM

// Video configuration
#define CONFIG_FRAMEBUFFER
#define CONFIG_GL

// Library configuration
#define CONFIG_BUILTIN_MM
#define CONFIG_BUILTIN_LIBC

// GBA/NDS specific configuration
#undef  CONFIG_GBA_KEYBOARD

// NDS specific configuration
#define CONFIG_NDS_IPC


#endif
