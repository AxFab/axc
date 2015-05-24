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

int _errno;

int *__errno_location(void)
{
  return &_errno;
}

int *_geterrno(void)
{
  return &_errno;
}

int __cpu_no(void) {
  return 0;
}

void __assert_fail(const char* assertion, const char *file, unsigned int line, const char *function)
{
  // fprintf(stderr, "Assertion: (%s) at %s, %s:%d\n", assertion, function, file, line);
  // abort(0);
}

void __assert_do(int as, const char *ex, const char *at)
{
  // if (!as)
  //   kpanic("Assertion: %s at %s.\n", ex, at);
}
