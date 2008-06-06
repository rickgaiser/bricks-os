#ifndef CONFIG_H
#define CONFIG_H


// Debugging
#define CONFIG_DEBUGGING

// Hardware capabilities
#define __LITTLE_ENDIAN__
//#define __BIG_ENDIAN__
#define CONFIG_FPU

// Kernel interface
#undef  CONFIG_DIRECT_ACCESS_KERNEL
#undef  CONFIG_DIRECT_ACCESS_KERNEL_FUNC

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


#endif
