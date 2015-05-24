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
#include <sys/types.h>

int __syscall(int no, ...);
#define syscall(no, ...)  __syscall(no, __VA_ARGS__)

void exit(int status)
{
  for (;;)
    syscall(0x01, status);
}

ssize_t read(unsigned int fd, char* buf, size_t lg)
{
  return syscall(0x03, fd, buf, lg);
}

ssize_t write(unsigned int fd, const char* buf, size_t lg)
{
  return syscall(0x04, fd, buf, lg);
}

int open(const char *path, int flags, int mode)
{
  return syscall(0x05, path, flags, mode);
}

void close(int fd)
{
  syscall(0x06, fd);
}

void brk(long add)
{
  syscall(0x2d, add);
}

void __getcwd(char *buf, long size)
{
  syscall(0xb7, buf, size);
}
