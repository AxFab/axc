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
#ifndef LIBIO_H__
#define LIBIO_H__

#include <stdarg.h>


struct _IO_jump_t; struct _IO_FILE;

typedef void _IO_lock_t;
struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;
  int _pos;

};
enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};

struct _IO_FILE {
  int _flags;
  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;
  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;
  struct _IO_marker *_markers;
  struct _IO_FILE *_chain;
  int _fileno;
  int _flags2;
  __off_t _old_offset;
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
  _IO_lock_t *_lock;

  __off64_t _offset;

  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;
  int _mode;
  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];
};
typedef struct _IO_FILE _IO_FILE;
struct _IO_FILE_plus;
struct _IO_FILE_plus _IO_2_1_stdin_;
struct _IO_FILE_plus _IO_2_1_stdout_;
struct _IO_FILE_plus _IO_2_1_stderr_;

typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);
typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
     size_t __n);
typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);
typedef int __io_close_fn (void *__cookie);

int __underflow (_IO_FILE *);
int __uflow (_IO_FILE *);
int __overflow (_IO_FILE *, int);

int _IO_getc (_IO_FILE *__fp);
int _IO_putc (int __c, _IO_FILE *__fp);
int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__));
int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__));
int _IO_peekc_locked (_IO_FILE *__fp);
void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__));
void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__));
int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__));

int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
__ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
size_t _IO_sgetn (_IO_FILE *, void *, size_t);
__off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
__off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);
void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__));

#endif /* LIBIO_H__ */
