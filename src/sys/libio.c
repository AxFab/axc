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
#include <string.h>
#include <limits.h>
#include <fcntl.h>

struct _IO_FILE *stdin;
struct _IO_FILE *stdout;
struct _IO_FILE *stderr;

#ifdef __USE_SCALL


static int _fwrite(struct _IO_FILE *fp, const void *buf, size_t length)
{
  if (1 /* fp->oflags_ & O_DIRECT*/) {
    return write(fp->fd_, buf, length);
  }

  return EOF;
}

static int _fread(struct _IO_FILE *fp, void *buf, size_t length)
{
  if (1 /* fp->oflags_ & O_DIRECT*/) {
    return read(fp->fd_, buf, length);
  }

  return EOF;
}

/* ----------------------------------------------------------------------- */
/** @brief Allocate a new FILE structure form an open file descriptor.
  *
  * @param fd [in] File descritpor of this file
  * @param oflags [in] Flags sended to the open function
  * @error ENOMEM, EBADF, EINVAL
  */
static struct _IO_FILE* _fvopen(int fd, int oflags)
{
  struct _IO_FILE* fp;
  if (fd < 0) {
    errno = EBADF;
    return NULL;
  }

  if (oflags < 0) {
    errno = EINVAL;
    return NULL;
  }

  fp = (struct _IO_FILE*)malloc (sizeof(*fp));
  if (!fp)
    return NULL;

  memset(fp, 0, sizeof(*fp));
  fp->fd_ = fd;
  fp->oflags_ = oflags | _IOLBF;
  fp->read_ = _fread;
  fp->write_ = _fwrite;
  return fp;
}

/** @brief Parse the character base mode for opening file. and return
  * binary mode
  *
  * @param mode [in] A character string that define an opening mode.
  * @return An int that contains opening flags, or zero if mode is invalid.
  *
  * The mode parameter must start by on of this sequence:
  *   r  Open the file for reading. The stream is positioned at the
  *        beginning of the file.
  *   r+ Open the file for reading and writing. The stream is positioned at
  *        the beginning of the file.
  *   w  Truncate file to zero length or create a new file for writing. The
  *        stream is positioned at the beginning of the file.
  *   w+ Open the file for reading and writing. The file is created if
  *        needed, or truncated. The stream is positioned at the beginning
  *        of the file.
  *   a  Open for appending (writing at the end). The file is created if it
  *        doesn't exist. The stream is positioned at the end of file.
  *   a+  Open for reading and appending (writing at the ent). The initial
  *        file position is at the beginning, but output is always append
  *        to the end.
  *
  * The mode can be completed by this extensions:
  *   b   (mingw32) Try to optimize for binary file
  *   x   (glibc) Open the file exclusively (like O_EXEC)
  *   e   (glibc) Close the file on exec
  *   m   (glibc) Try to use mmap
  *   F   (uClibc) Open a a large file
  *
  * Note: on this implementation repeated or unknown extensions don't cause any
  *       errors and '+' is consider as and extensions rather than a mode suffix.
  */
static int _foflags (const char* mode)
{
  int oflags = 0;
  if (*mode == 'r') {
    oflags |= O_RDONLY;
  } else if (*mode == 'w') {
    oflags |= O_WRONLY | O_CREAT | O_TRUNC;
  } else if (*mode == 'a') {
    oflags |= O_WRONLY | O_CREAT | O_APPEND;
  } else {
    errno = EINVAL;
    return -1;
  }

  ++mode;
  while (*mode != '\0' && *mode != ',') {
    switch (*mode) {
      case 'r':
      case 'w':
      case 'a':
        errno = EINVAL;
        return -1;

      case '+':
        oflags &= ~(O_RDONLY | O_WRONLY);
        oflags |= O_RDWR;
        break;

      // case 'b': oflags |= O_BINARY; break;
      case 'x': oflags |= O_EXCL; break;
      case 'e': oflags |= O_CLOEXEC; break;
      // case 'm': oflags |= O_MEMORYMAP; break;
      // case 'F': oflags |= O_LARGEFILE; break;
    }
    ++mode;
  }

  errno = 0;
  return oflags;
}

struct _IO_FILE *fopen(const char *path, const char *mode)
{
  int md;
  int fd;
  struct _IO_FILE *fp;

  md = _foflags(mode);
  if (md < 0 || path == NULL)
    return NULL;

  fd = open(path, md);
  if (fd == 0)
    return NULL;

  fp = _fvopen(fd, md);
  if (fp == NULL) {
    close (fd);
    fp = NULL;
  }

  return fp;
}


/** Create a stream for a opened file */
struct _IO_FILE* fdopen(int fd, const char* mode)
{
  int oflags;

  /* Get openning flags */
  oflags = _foflags(mode);
  if (!oflags) {
    return NULL;
  }

  return _fvopen(fd, oflags);
}


int fclose(struct _IO_FILE* fp)
{
  if (fp->write_ != _fwrite || fp->read_ != _fread) {
    errno = EBADF;
    return -1;
  }

  FLOCK(fp);
  /* flush buffers */
  close(fp->fd_);
  FRMLOCK(fp);
  free(fp);
  return 0;
}

#endif

/* ----------------------------------------------------------------------- */

size_t fread(void *ptr, size_t sz, size_t cnt, struct _IO_FILE* fp)
{
  FLOCK(fp);
  size_t items = 0;
  for (items = 0; items < cnt; ++items) {
    if (fp->read_(fp, ptr, sz) == EOF)
      break;
  }

  FUNLOCK(fp);
  return items;
}

size_t fwrite(const void *ptr, size_t sz, size_t cnt, struct _IO_FILE* fp)
{
  FLOCK(fp);
  size_t items = 0;
  for (items = 0; items < cnt; ++items) {
    if (fp->write_(fp, ptr, sz) == EOF)
      break;
    ptr += sz;
  }

  FUNLOCK(fp);
  return items;
}

/* ----------------------------------------------------------------------- */

int fgetc_unlocked(struct _IO_FILE *fp)
{
  int c;
  if (fp->read_(fp, &c, 1) == EOF) {
    return EOF;
  }

  return c;
}

/**
  * @return s in success, NULL on error, EOF if nothing readed. */
char *fgets_unlocked(char *s, int size, struct _IO_FILE *fp)
{
  int c;
  unsigned char *ps = (unsigned char *)s;

  while (--size) {
    c = fgetc_unlocked(fp);
    if (c == EOF) {
      if (errno)
        return NULL;
      break;
    }

    *ps = (unsigned char)c;
    if (c == '\n')
      break;

    ++ps;
  }

  *ps = '\0';
  return ((void*)s == (void*)ps) ? (char*)EOF : s;
}

int fgetc(struct _IO_FILE *fp)
{
  int c;
  FLOCK(fp);
  c = fgetc_unlocked(fp);
  FUNLOCK(fp);
  return c;
}

char *fgets(char *s, int size, struct _IO_FILE *fp)
{
  FLOCK(fp);
  s = fgets_unlocked(s, size, fp);
  FUNLOCK(fp);
  return s;
}

int getc(struct _IO_FILE *fp)
{
  return fgetc(fp);
}

int getchar(void)
{
  return fgetc(stdin);
}

char *gets(char *s)
{
  return fgets(s, INT_MAX, stdin);
}

int ungetc(int c, struct _IO_FILE *fp)
{
  return EOF;
}

/* ----------------------------------------------------------------------- */

int fputc(int c, struct _IO_FILE *fp)
{
  size_t ret = fwrite(&c, 1, 1, fp);
  return ((int)ret == 1) ? (unsigned char)c : EOF;
}

int fputs(const char *s, struct _IO_FILE *fp)
{
  int lg = strlen(s);
  size_t ret = fwrite(s, lg, 1, fp);
  return ((int)ret == lg) ? lg : EOF;
}

int putc(int c, struct _IO_FILE *fp)
{
  return fputc(c, fp);
}

int putchar(int c)
{
  return fputc(c, stdout);
}

int puts(const char *s)
{
  char *ps = (char*)s;
  int lg = strlen(s);
  size_t ret;
  ps[lg] = '\n';
  ret = fwrite(ps, lg, 1, stdout);
  ps[lg] = '\0';
  return ((int)ret == lg) ? lg : EOF;
}

/* ----------------------------------------------------------------------- */

void clearerr (struct _IO_FILE* fp)
{
  fp->oflags_ &= ~(OF_ERR | OF_EOF);
}

int feof (struct _IO_FILE* fp)
{
  return fp->oflags_ & OF_EOF;
}


int ferror (struct _IO_FILE* fp)
{
  return fp->oflags_ & OF_ERR;
}


int fileno (struct _IO_FILE* fp)
{
  return fp->fd_;
}

/* ----------------------------------------------------------------------- */

void __axc_open(void)
{
  stdin = _fvopen (0, O_RDONLY);
  stdout = _fvopen (1, O_WRONLY);
  stderr = _fvopen (2, O_WRONLY);
}

