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
#ifndef TIME_H__
#define TIME_H__

#include <features.h>
#include <stddef.h>
#include <bits/time.h>
#include <bits/types.h>

typedef __clock_t clock_t;

typedef __time_t time_t;

typedef __clockid_t clockid_t;

typedef __timer_t timer_t;

struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  long int tm_gmtoff;
  __const char *tm_zone;
};
struct itimerspec
  {
    struct timespec it_interval;
    struct timespec it_value;
  };
struct sigevent;
typedef __pid_t pid_t;
clock_t clock (void) __attribute__ ((__nothrow__));
time_t time (time_t *__timer) __attribute__ ((__nothrow__));
double difftime (time_t __time1, time_t __time0)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__));
time_t mktime (struct tm *__tp) __attribute__ ((__nothrow__));
size_t strftime (char *__restrict __s, size_t __maxsize,
   __const char *__restrict __format,
   __const struct tm *__restrict __tp) __attribute__ ((__nothrow__));

#include <xlocale.h>


size_t strftime_l (char *__restrict __s, size_t __maxsize,
     __const char *__restrict __format,
     __const struct tm *__restrict __tp,
     __locale_t __loc) __attribute__ ((__nothrow__));


struct tm *gmtime (__const time_t *__timer) __attribute__ ((__nothrow__));
struct tm *localtime (__const time_t *__timer) __attribute__ ((__nothrow__));
struct tm *gmtime_r (__const time_t *__restrict __timer,
       struct tm *__restrict __tp) __attribute__ ((__nothrow__));
struct tm *localtime_r (__const time_t *__restrict __timer,
          struct tm *__restrict __tp) __attribute__ ((__nothrow__));
char *asctime (__const struct tm *__tp) __attribute__ ((__nothrow__));
char *ctime (__const time_t *__timer) __attribute__ ((__nothrow__));
char *asctime_r (__const struct tm *__restrict __tp,
   char *__restrict __buf) __attribute__ ((__nothrow__));
char *ctime_r (__const time_t *__restrict __timer,
        char *__restrict __buf) __attribute__ ((__nothrow__));
char *__tzname[2];
int __daylight;
long int __timezone;
char *tzname[2];
void tzset (void) __attribute__ ((__nothrow__));
int daylight;
long int timezone;
int stime (__const time_t *__when) __attribute__ ((__nothrow__));


time_t timegm (struct tm *__tp) __attribute__ ((__nothrow__));
time_t timelocal (struct tm *__tp) __attribute__ ((__nothrow__));
int dysize (int __year) __attribute__ ((__nothrow__)) __attribute__ ((__const__));


int nanosleep (__const struct timespec *__requested_time,
        struct timespec *__remaining);
int clock_getres (clockid_t __clock_id, struct timespec *__res) __attribute__ ((__nothrow__));
int clock_gettime (clockid_t __clock_id, struct timespec *__tp) __attribute__ ((__nothrow__));
int clock_settime (clockid_t __clock_id, __const struct timespec *__tp)
     __attribute__ ((__nothrow__));
int clock_nanosleep (clockid_t __clock_id, int __flags,
       __const struct timespec *__req,
       struct timespec *__rem);
int clock_getcpuclockid (pid_t __pid, clockid_t *__clock_id) __attribute__ ((__nothrow__));
int timer_create (clockid_t __clock_id,
    struct sigevent *__restrict __evp,
    timer_t *__restrict __timerid) __attribute__ ((__nothrow__));
int timer_delete (timer_t __timerid) __attribute__ ((__nothrow__));
int timer_settime (timer_t __timerid, int __flags,
     __const struct itimerspec *__restrict __value,
     struct itimerspec *__restrict __ovalue) __attribute__ ((__nothrow__));
int timer_gettime (timer_t __timerid, struct itimerspec *__value)
     __attribute__ ((__nothrow__));
int timer_getoverrun (timer_t __timerid) __attribute__ ((__nothrow__));

#endif /* TIME_H__ */
