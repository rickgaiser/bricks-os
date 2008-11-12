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


#ifndef ERRNO_H
#define ERRNO_H


#define E2BIG                1
#define ACCES                2
#define EADDRINUSE           3
#define EADDRNOTAVAIL        4
#define EAFNOSUPPORT         5
#define EAGAIN               6
#define EALREADY             7
#define EBADF                8
#define EBADMSG              9
#define EBUSY               10
#define ECANCELED           11
#define ECHILD              12
#define ECONNABORTED        13
#define ECONNREFUSED        14
#define ECONNRESET          15
#define EDEADLK             16
#define EDESTADDRREQ        17
#define EDOM                18
#define EDQUOT              19
#define EEXIST              20
#define EFAULT              21
#define EFBIG               22
#define EHOSTUNREACH        23
#define EIDRM               24
#define EILSEQ              25
#define EINPROGRESS         26
#define EINTR               27
#define EINVAL              28
#define EIO                 29
#define EISCONN             30
#define EISDIR              31
#define ELOOP               32
#define EMFILE              33
#define EMLINK              34
#define EMSGSIZE            35
#define EMULTIHOP           36
#define ENAMETOOLONG        37
#define ENETDOWN            38
#define ENETUNREACH         39
#define ENFILE              40
#define ENOBUFS             41
#define ENODATA             42
#define ENODEV              43
#define ENOENT              44
#define ENOEXEC             45
#define ENOLCK              46
#define ENOLINK             47
#define ENOMEM              48
#define ENOMSG              49
#define ENOPROTOOPT         50
#define ENOSPC              51
#define ENOSR               52
#define ENOSTR              53
#define ENOSYS              54
#define ENOTCONN            55
#define ENOTDIR             56
#define ENOTEMPTY           57
#define ENOTSOCK            58
#define ENOTSUP             59
#define ENOTTY              60
#define ENXIO               61
#define EOPNOTSUPP          62
#define EOVERFLOW           63
#define EPERM               64
#define EPIPE               65
#define EPROTO              66
#define EPROTONOSUPPORT     67
#define EPROTOTYPE          68
#define ERANGE              69
#define EROFS               70
#define ESPIPE              71
#define ESRCH               72
#define ESTALE              73
#define ETIME               74
#define ETIMEDOUT           75
#define ETXTBSY             76
#define EWOULDBLOCK         77
#define EXDEV               78


extern int errno;


#endif
