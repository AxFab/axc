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

#define __byte unsigned char


/* ----------------------------------------------------------------------- */
/** Copy block of memory */
void *memcpy ( void *dest, const void *src, size_t length )
{
  register __byte *ptr1 = ( __byte *) dest;
  register const __byte *ptr2 = ( const __byte *) src;

  while (length--) {
    *ptr1++ = *ptr2++;
  }

  return dest;
}

/* ----------------------------------------------------------------------- */
/** Copy source buffer to destination buffer */
void *memmove ( void *dest, const void *src, size_t length )
{
  register __byte *ptr1 = ( __byte *) dest;
  register const __byte *ptr2 = ( const __byte *) src;

  if ( ptr1 >= ptr2 || ptr1 >= ptr2 + length ) {
    while (length--) {
      *ptr1++ = *ptr2++;
    }
  } else {
    ptr1 += length - 1;
    ptr2 += length - 1;

    while (length--) {
      *ptr1-- = *ptr2--;
    }
  }

  return dest;
}

/* ----------------------------------------------------------------------- */
/** Compare two blocks of memory */
int memcmp ( const void *dest, const void *src, size_t length )
{
  register const __byte *ptr1 = ( const __byte *) dest;
  register const __byte *ptr2 = ( const __byte *) src;

  while ( --length && *ptr1 == *ptr2 ) {
    ++ptr1;
    ++ptr2;
  }

  return *ptr1 - *ptr2;
}

/* ----------------------------------------------------------------------- */
/** Search a character into a block of memory */
void *memchr ( const void *ptr, int chr, size_t length )
{
  register const __byte *ptr0 = ( const __byte *) ptr;

  while ( length > 0 && ( *ptr0 != ( __byte ) chr ) ) {
    ++ptr0;
    --length;
  }

  return ( void *) ( length ? ptr0 : 0 );
}

/* ----------------------------------------------------------------------- */
/** Set all byte of a block of memory to the same value */
void *memset ( void *ptr, int val, size_t length )
{
  register __byte *org = ( __byte *) ptr;

  while (length--) {
    *org++ = ( __byte ) val;
  }

  return ptr;
}



/* return length of a null-terminated char string */
size_t strlen ( const char *str )
{
  register const char *end = str;

  while ( *end ) {
    ++end;
  }

  return end - str;
}

/* return length of a null-terminated char string */
size_t strnlen ( const char *str, size_t length )
{
  register size_t count;

  for ( count = 0; count < length && *str; ++str );

  return count;
}



// Copy a null-terminated char string
char *strcpy ( char *dest, const char *src )
{
  register char *ptr1 = ( char *) dest;
  register const char *ptr2 = ( const char *) src;

  while ( ( *ptr1++ = *ptr2++ ) );

  return dest;
}

// Copy a char string
char *strncpy ( char *dest, const char *src, size_t length )
{
  register char *ptr1 = ( char *) dest;
  register const char *ptr2 = ( const char *) src;

  while ( length-- > 0 && ( *ptr1++ = *ptr2++ ) );

  return dest;
}

// Concat two null-terminated char strings
char *strcat ( char *dest, const char *src )
{
  register char *ptr1 = ( char *) dest;
  register const char *ptr2 = ( const char *) src;

  while ( *ptr1 ) {
    ++ptr1;
  }

  while ( ( *ptr1++ = *ptr2++ ) );

  return dest;
}

// Concat two null-terminated char strings
char *strncat ( char *dest, const char *src, size_t length )
{
  register char *ptr1 = ( char *) dest;
  register const char *ptr2 = ( const char *) src;

  while ( *ptr1 ) {
    ++ptr1;
  }

  while ( length-- > 0 && ( *ptr1++ = *ptr2++ ) );

  return dest;
}

// Compare two null-terminated char strings
int strcmp ( const char *str1, const char *str2 )
{
  while ( *str1 && ( *str1 == *str2 ) ) {
    ++str1;
    ++str2;
  }

  return *str1 - *str2;
}

// Compare two char strings
int strncmp ( const char *str1, const char *str2, size_t length )
{
  while ( --length && *str1 && *str1 == *str2 ) {
    ++str1;
    ++str2;
  }

  return *str1 - *str2;
}


// ---------------------------------------------------------------------------
// Search a string for a character
char *strchr (const char *string, int ch)
{
  while (*string) {
    if (*string == (char) ch) {
      return (char *) string;
    }

    string++;
  }

  return NULL;
}

// ---------------------------------------------------------------------------
// Search a string for a character
char *strrchr (const char *string, int ch)
{
  int lg = strlen (string) - 1;

  for (; lg >= 0; --lg) {
    if (string[lg] == (char) ch) {
      return (char *) string;
    }
  }

  return NULL;
}


// ---------------------------------------------------------------------------
// Split string into tokens - reentrent
/// @todo suppress goto and that is not the best way
char *strtok_r ( register char *s, register const char *delim, char **lasts )
{
  int skip_leading_delim = 1;
  register char *spanp;
  register int c, sc;
  char *tok;

  if ( s == NULL && ( s = *lasts ) == NULL ) {
    return NULL;
  }

  /*
   * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
   */
cont:
  c = *s++;

  for ( spanp = ( char *) delim; ( sc = *spanp++ ) != 0; ) {
    if ( c == sc ) {
      if ( skip_leading_delim ) {
        goto cont;
      } else {
        *lasts = s;
        s[-1] = 0;
        return ( s - 1 );
      }
    }
  }

  if ( c == 0 ) {      /* no non-delimiter characters */
    *lasts = NULL;
    return NULL;
  }

  tok = s - 1;

  /*
   * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
   * Note that delim must have one NUL; we stop if we see that, too.
   */
  for ( ;; ) {
    c = *s++;
    spanp = ( char *) delim;

    do {
      if ( ( sc = *spanp++ ) == c ) {
        if ( c == 0 ) {
          s = NULL;
        } else {
          s[-1] = 0;
        }

        *lasts = s;
        return ( tok );
      }
    } while ( sc != 0 );
  }

  /* NOTREACHED */
}

static char *strtok_reent = 0;

// Split string into tokens
char *strtok ( register char *string , register const char *delimitors )
{
  return strtok_r ( string, delimitors, &strtok_reent );
}

