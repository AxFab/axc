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
#ifndef AXC_LIBIO_H__
#define AXC_LIBIO_H__

#include <errno.h>
#include <stddef.h>
#include <axc/cc.h>

struct _IO_FILE
{
  int     fd_;
  int     oflags_;
  struct _IO_FILE *next_;

  int     lock_;
  int     count_;
  char    lbuf_;

  char   *buf_;
  size_t  off_;
  size_t  lpos_;
  size_t  bsize_;

  char   *rpos_;
  char   *rend_;
  char   *wpos_;
  char   *wend_;

  int (*write_) (struct _IO_FILE *fp, const void *buf, size_t length);
  int (*read_) (struct _IO_FILE *fp, void *buf, size_t length);
};

struct _IO_FSPEC
{
  int flag_;
  int type_;
  int precis_;
  int field_;
};

union __IO_FTYPE {
  void *p;
  int s;
  unsigned int i;
  double f;
};


extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;

#define EOF (-1)
#define FLOCK(f) ((void)fp)
#define FUNLOCK(f) ((void)fp)
#define FRMLOCK(f) ((void)fp)


#define _IOLBF  1
#define OF_ERR 1
#define OF_EOF 2


int open(const char *path, int oflags, ...);
void close(int fd);
size_t read(int fd, void *buf, size_t lg);
size_t write(int fd, const void *buf, size_t lg);

#endif /* AXC_LIBIO_H__ */