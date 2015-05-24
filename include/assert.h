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
#ifndef ASSERT_H__
#define ASSERT_H__

#include <features.h>

#ifdef NDEBUG

// assert call will be erase by the compiler
# define assert(ignore) ((void) 0)

#else

/** assert function is called if test is true */
#  define assert(e)       __assert_do((int)(e),#e, __AT__)
#  define assert_msg(e,m)   __assert_do((int)(e),m, __AT__)
#  define DEBUG(c)    do { c } while(0)

/** If the assertion test is false, the program will print the reason for
  * the assertion an leave the program. in other case this routines have no
  * effect. */
void __assert_do(int as, const char *ex, const char *at);

#endif

#endif /* ASSERT_H__ */
