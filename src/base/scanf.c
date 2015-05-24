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
#include <axc/format.h>
#include <string.h>
#include <stdarg.h>

int vfscanf(struct _IO_FILE *restrict f, const char *restrict format, va_list ap);

/* ----------------------------------------------------------------------- */
/** Read from a string streaming */
static int _sread(struct _IO_FILE *restrict fp, void* restrict buf, size_t length)
{
  size_t lg = MIN (length, (size_t)(fp->rend_ - fp->rpos_));
  memcpy (buf, fp->rpos_, lg);
  fp->rpos_ += lg;
  return length > lg ? EOF : (int)lg;
}


// ===========================================================================
/** Read and parse standard input */
int scanf(const char *restrict format, ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);
  ret = vfscanf(stdin, format, ap);
  va_end(ap);
  return ret;
}


// ---------------------------------------------------------------------------
/** Read and parse an open file */
int fscanf(struct _IO_FILE *restrict f, const char *restrict format, ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);
  ret = vfscanf(f, format, ap);
  va_end(ap);
  return ret;
}


// ---------------------------------------------------------------------------
/** Read and parse standard input */
int vscanf(const char *restrict format, va_list ap)
{
  return vfscanf(stdin, format, ap);
}


// ===========================================================================
/** Read and parse a string */
int sscanf(const char *restrict str, const char *restrict format, ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);
  struct _IO_FILE fp = {
    .rpos_ = (char*)str,
    .rend_ = (char*)SIZE_MAX,
    .read_ = _sread,
    .lock_ = -1
  };
  ret = vfscanf(&fp, format, ap);
  va_end(ap);
  return ret;
}


// ---------------------------------------------------------------------------
/** Read and parse a string */
int vsscanf(const char *restrict str, const char *restrict format, va_list ap)
{
  struct _IO_FILE fp = {
    .rpos_ = (char*)str,
    .rend_ = (char*)SIZE_MAX,
    .read_ = _sread,
    .lock_ = -1
  };
  return vfscanf(&fp, format, ap);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
