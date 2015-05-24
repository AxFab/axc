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
#include <stddef.h>
#include <stdint.h>

// ---------------------------------------------------------------------------
uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem_p)
{
  uint64_t quot = 0, qbit = 1;

  if ( den == 0 )
    return 1 / ((unsigned)den); /* INT: divide by zero */

  /* Left-justify denominator and count shift */
  while ( (int64_t)den >= 0 ) {
    den <<= 1;
    qbit <<= 1;
  }

  while ( qbit ) {
    if ( den <= num ) {
      num -= den;
      quot += qbit;
    }

    den >>= 1;
    qbit >>= 1;
  }

  if ( rem_p )
    *rem_p = num;

  return quot;
}


// ---------------------------------------------------------------------------
int64_t __divdi3(int64_t num, int64_t den)
{
  int minus = 0;
  int64_t v;

  if ( num < 0 ) {
    num = -num;
    minus = 1;
  }

  if ( den < 0 ) {
    den = -den;
    minus ^= 1;
  }

  v = __udivmoddi4(num, den, NULL);

  if ( minus )
    v = -v;

  return v;
}


// ---------------------------------------------------------------------------
int64_t __moddi3(int64_t num, int64_t den)
{
  int minus = 0;
  int64_t v;

  if ( num < 0 ) {
    num = -num;
    minus = 1;
  }

  if ( den < 0 ) {
    den = -den;
    minus ^= 1;
  }

  __udivmoddi4(num, den, (uint64_t *)&v);

  if ( minus )
    v = -v;

  return v;
}


// ---------------------------------------------------------------------------
uint64_t __udivdi3(uint64_t num, uint64_t den)
{
  return __udivmoddi4(num, den, NULL);
}


// ---------------------------------------------------------------------------
uint64_t __umoddi3(uint64_t num, uint64_t den)
{
  uint64_t v;

  __udivmoddi4(num, den, &v);
  return v;
}
