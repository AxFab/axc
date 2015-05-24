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
#include <axc/format.h>
#include <string.h>
#include <stdarg.h>

/* All of those methods are bind over vfprintf
 * which is implemented in another file.
 */
int vfprintf(struct _IO_FILE *fp, const char *str, va_list ap);
int vsnprintf(char *restrict str, size_t lg, const char *restrict format, va_list ap);

// ---------------------------------------------------------------------------
/** Write on a string streaming */
static int _swrite (struct _IO_FILE * fp, const void *restrict buf, size_t length)
{
  size_t lg = MIN (length, (size_t)(fp->wend_ - fp->wpos_));
  memcpy (fp->wpos_, buf, lg);
  fp->wpos_ += lg;
  fp->count_ += lg;
  return (length > lg) ? EOF : (int)lg;
}

#ifdef __USE_SCALL
/** Write on a a file descriptor */
static int _dwrite (struct _IO_FILE * fp, const void *restrict buf, size_t lg)
{
  size_t cnt = write (fp->fd_, buf, lg);
  return (cnt > lg) ? EOF : (int)lg;
}
// ---------------------------------------------------------------------------


/** Write formated string on standard output */
int printf(const char *restrict format, ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);
  ret = vfprintf(stdout, format, ap);
  va_end(ap);
  return ret;
}


/** Write formated string on standard output */
int vprintf(const char *restrict format, va_list ap)
{
  return vfprintf(stdout, format, ap);
}


/** Write formated string on an opened file */
int fprintf(struct _IO_FILE *restrict fp, const char *restrict format, ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);
  ret = vfprintf(fp, format, ap);
  va_end(ap);
  return ret;
}





// ---------------------------------------------------------------------------
/** Write formated string from a file descriptor */
int vdprintf(int fd, const char *restrict format, va_list ap)
{
  struct _IO_FILE fp = {
    .fd_ = fd,
    .lbuf_ = EOF,
    .write_ = _dwrite,
    .lock_ = -1
  };
  return vfprintf(&fp, format, ap);
}


/** Write formated string from a file descriptor */
int dprintf(int fd, const char *restrict format, ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);
  ret = vdprintf(fd, format, ap);
  va_end(ap);
  return ret;
}

#endif

// ===========================================================================
/** Write formated string from a string */
int sprintf(char *restrict str, const char *restrict format, ...)
{
  int res;
  va_list ap;
  va_start(ap, format);
  res = vsnprintf (str, INT_MAX, format, ap);
  va_end(ap);
  return res;
}


// ---------------------------------------------------------------------------
/** Write formated string from a string */
int snprintf(char *restrict str, size_t lg, const char *restrict format, ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);
  ret = vsnprintf(str, lg, format, ap);
  va_end(ap);
  return ret;
}


// ---------------------------------------------------------------------------
/** Write formated string from a string */
int vsprintf(char *restrict str, const char *restrict format, va_list ap)
{
  return vsnprintf(str, INT_MAX, format, ap);
}


// ---------------------------------------------------------------------------
/** Write formated string from a string */
int vsnprintf(char *restrict str, size_t lg, const char *restrict format, va_list ap)
{
  char b;
  int res;
  struct _IO_FILE fp = {
    .lbuf_ = EOF,
    .write_ = _swrite,
    .lock_ = -1,
    .wpos_ = str,
    .wend_ = str + lg,
  };

  if (lg - 1 > INT_MAX - 1) {
    errno = EOVERFLOW;
    return -1;
  } else if (!lg) {
    fp.wpos_ = &b;
    fp.wend_ = fp.wpos_++;
  } else if (fp.wend_ < fp.wpos_) {
    fp.wend_ = (char *)SIZE_MAX;
  }

  res = vfprintf(&fp, format, ap);
  fp.wpos_[-(fp.wpos_ == fp.wend_)] = '\0';
  return res;
}


// ---------------------------------------------------------------------------
/** Write formated string from an allocated string */
int vasprintf(char **s, const char *format, va_list ap)
{
  va_list ap2;
  va_copy(ap2, ap);
  int l = vfprintf(NULL, format, ap2);
  va_end(ap2);

  if (l < 0 || !(*s = malloc(l + 1)))
    return -1;

  return vsnprintf(*s, l + 1, format, ap);
}


/** Write formated string from an allocated string */
int asprintf(char **str, const char *format, ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);
  ret = vasprintf(NULL, format, ap);
  va_end(ap);
  return ret;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
