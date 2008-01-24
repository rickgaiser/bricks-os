#ifndef CONFIG_H
#define CONFIG_H


// Hardware capabilities
#define CONFIG_FPU

// Kernel interface
#define CONFIG_DIRECT_ACCESS_KERNEL
#define CONFIG_DIRECT_ACCESS_KERNEL_FUNC

// File System
#define CONFIG_FILESYSTEM
#define CONFIG_FS_FAT
#define CONFIG_PD_IBM

// Video configuration
#undef  CONFIG_FRAMEBUFFER
#undef  CONFIG_GL
#undef  CONFIG_GL_MULTI_CONTEXT

// Library configuration
#define CONFIG_BUILTIN_MM
#define CONFIG_BUILTIN_LIBC


#endif
