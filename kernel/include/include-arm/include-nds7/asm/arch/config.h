#ifndef CONFIG_H
#define CONFIG_H


// Debugging
#undef  CONFIG_DEBUGGING

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
#undef  CONFIG_FILESYSTEM
#undef  CONFIG_FS_FAT
#undef  CONFIG_PD_IBM

// Video configuration
#undef  CONFIG_FRAMEBUFFER
#undef  CONFIG_GL

// Library configuration
#define CONFIG_BUILTIN_MM
#define CONFIG_BUILTIN_LIBC

// GBA/NDS specific configuration
#define CONFIG_GBA_KEYBOARD

// NDS specific configuration
#define CONFIG_NDS_IPC


#endif
