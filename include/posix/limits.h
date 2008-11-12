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


#ifndef LIMITS_H
#define LIMITS_H


#ifndef ULONG_MAX
#define ULONG_MAX ((unsigned long) ~(unsigned long) 0)
#define ULONG_MIN ((unsigned long) 0 - ULONG_MAX)
#endif

#ifndef LONG_MAX
#define LONG_MAX ((long) (ULONG_MAX >> 1))
#define LONG_MIN ((long) (0 - LONG_MAX))
#endif


#endif
