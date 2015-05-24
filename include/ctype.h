/*
 *      This file is part of the SmokeOS project.
 *  Copyright (C) 2015  <Fabien Bavent>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifndef CTYPE_H__
#define CTYPE_H__

#include <features.h>
#include <bits/types.h>
#include <endian.h>

#include <xlocale.h>

enum
{
  _ISupper = ((0) < 8 ? ((1 << (0)) << 8) : ((1 << (0)) >> 8)),
  _ISlower = ((1) < 8 ? ((1 << (1)) << 8) : ((1 << (1)) >> 8)),
  _ISalpha = ((2) < 8 ? ((1 << (2)) << 8) : ((1 << (2)) >> 8)),
  _ISdigit = ((3) < 8 ? ((1 << (3)) << 8) : ((1 << (3)) >> 8)),
  _ISxdigit = ((4) < 8 ? ((1 << (4)) << 8) : ((1 << (4)) >> 8)),
  _ISspace = ((5) < 8 ? ((1 << (5)) << 8) : ((1 << (5)) >> 8)),
  _ISprint = ((6) < 8 ? ((1 << (6)) << 8) : ((1 << (6)) >> 8)),
  _ISgraph = ((7) < 8 ? ((1 << (7)) << 8) : ((1 << (7)) >> 8)),
  _ISblank = ((8) < 8 ? ((1 << (8)) << 8) : ((1 << (8)) >> 8)),
  _IScntrl = ((9) < 8 ? ((1 << (9)) << 8) : ((1 << (9)) >> 8)),
  _ISpunct = ((10) < 8 ? ((1 << (10)) << 8) : ((1 << (10)) >> 8)),
  _ISalnum = ((11) < 8 ? ((1 << (11)) << 8) : ((1 << (11)) >> 8))
};

__const unsigned short int **__ctype_b_loc (void)
     __attribute__ ((__nothrow__)) __attribute__ ((__const));
__const __int32_t **__ctype_tolower_loc (void)
     __attribute__ ((__nothrow__)) __attribute__ ((__const));
__const __int32_t **__ctype_toupper_loc (void)
     __attribute__ ((__nothrow__)) __attribute__ ((__const));


int isalnum (int) __attribute__ ((__nothrow__));
int isalpha (int) __attribute__ ((__nothrow__));
int iscntrl (int) __attribute__ ((__nothrow__));
int isdigit (int) __attribute__ ((__nothrow__));
int islower (int) __attribute__ ((__nothrow__));
int isgraph (int) __attribute__ ((__nothrow__));
int isprint (int) __attribute__ ((__nothrow__));
int ispunct (int) __attribute__ ((__nothrow__));
int isspace (int) __attribute__ ((__nothrow__));
int isupper (int) __attribute__ ((__nothrow__));
int isxdigit (int) __attribute__ ((__nothrow__));
int tolower (int __c) __attribute__ ((__nothrow__));
int toupper (int __c) __attribute__ ((__nothrow__));
int isblank (int) __attribute__ ((__nothrow__));
int isascii (int __c) __attribute__ ((__nothrow__));
int toascii (int __c) __attribute__ ((__nothrow__));
int _toupper (int) __attribute__ ((__nothrow__));
int _tolower (int) __attribute__ ((__nothrow__));


int isalnum_l (int, __locale_t) __attribute__ ((__nothrow__));
int isalpha_l (int, __locale_t) __attribute__ ((__nothrow__));
int iscntrl_l (int, __locale_t) __attribute__ ((__nothrow__));
int isdigit_l (int, __locale_t) __attribute__ ((__nothrow__));
int islower_l (int, __locale_t) __attribute__ ((__nothrow__));
int isgraph_l (int, __locale_t) __attribute__ ((__nothrow__));
int isprint_l (int, __locale_t) __attribute__ ((__nothrow__));
int ispunct_l (int, __locale_t) __attribute__ ((__nothrow__));
int isspace_l (int, __locale_t) __attribute__ ((__nothrow__));
int isupper_l (int, __locale_t) __attribute__ ((__nothrow__));
int isxdigit_l (int, __locale_t) __attribute__ ((__nothrow__));
int isblank_l (int, __locale_t) __attribute__ ((__nothrow__));
int __tolower_l (int __c, __locale_t __l) __attribute__ ((__nothrow__));
int tolower_l (int __c, __locale_t __l) __attribute__ ((__nothrow__));
int __toupper_l (int __c, __locale_t __l) __attribute__ ((__nothrow__));
int toupper_l (int __c, __locale_t __l) __attribute__ ((__nothrow__));


#define isalnum(m) ((*__ctype_b_loc())[(unsigned int)(m)] & _ISalnum)

#define ispunct(m) ((*__ctype_b_loc())[(unsigned int)(m)] & _ISpunct)
#define isspace(m) ((*__ctype_b_loc())[(unsigned int)(m)] & _ISspace)


#endif /* CTYPE_H__ */
