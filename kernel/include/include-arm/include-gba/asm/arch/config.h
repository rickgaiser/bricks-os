/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


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
#undef  CONFIG_FILESYSTEM
#undef  CONFIG_FS_FAT
#undef  CONFIG_PD_IBM

// Video configuration
#define CONFIG_FRAMEBUFFER
#define CONFIG_GL
#undef  CONFIG_GL_PERSPECTIVE_CORRECT_TEXTURES
#undef  CONFIG_GL_ENABLE_ALPHA_TEST
#define CONFIG_GL_FRAMEBUFFER_16BIT
#define CONFIG_GL_TEXTURES_16BIT // NOTE: Only used when NOT using SIMPLE textures
#define CONFIG_GL_SIMPLE_TEXTURES

// Library configuration
#define CONFIG_BUILTIN_MM
#define CONFIG_BUILTIN_LIBC

// GBA/NDS specific configuration
#define CONFIG_GBA_KEYBOARD

// GBA specific configuration
#define CONFIG_GBA_SERIAL
#undef  CONFIG_GBA_SOUND


#endif
