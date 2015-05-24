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
#ifndef LIMITS_H__
#define LIMITS_H__

#include <syslimits.h>
#include <features.h>
#include <bits/posix_lim.h>
#include <bits/local_lim.h>


#define PATH_MAX 4096



#define SHRT_MIN (-SHRT_MAX-1)
#define SHRT_MAX 32767
#define USHRT_MAX 65536

#define INT_MIN (-INT_MAX-1)
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U

#define LONG_MIN INT_MIN
#define LONG_MAX INT_MAX
#define ULONG_MAX UINT_MAX

#define LLONG_MIN (-LLONG_MAX-1)
#define LLONG_MAX 9223372036854775807LL
#define ULLONG_MAX 18446744073709551615ULL


#endif /* LIMITS_H__ */
