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
#include <axc/libio.h>
#include <axc/alloc.h>
#include <axc/cc.h>
#include <stddef.h>

int main(int argc, char** argv, char**env);
void __axc_heap(void);
void __axc_open(void);
void exit(int);

long brk(long add);

void __axc_start(void)
{
  int argc = 0;
  char **argv = NULL;
  char **env = NULL;

  /* Initialize heap */
  __axc_heap();

  /* Initialize files */
  __axc_open();

  exit(main(argc, argv, env));
}
