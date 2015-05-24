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

#define __byte unsigned char

/**
 * @brief Set a suite of bits on a single byte
 */
int bsetbyte (__byte *byte, int off, int lg)
{
  __byte v = byte[0];
  int mask = (0xFF << off) & 0xFF;

  if (lg + off < 8) {
    mask = (mask & ~(0xFF << (off + lg))) & 0xFF;
  }

  byte[0] = v | mask;
  return v & mask;
}

/**
 * @brief Clear a suite of bits on a single byte
 */
int bclearbyte (__byte *byte, int off, int lg)
{
  __byte v = byte[0];
  int mask = (0xFF << off) & 0xFF;

  if (lg + off < 8) {
    mask = (mask & ~(0xFF << (off + lg))) & 0xFF;
  }

  byte[0] = v & ~mask;
  return (v ^ mask) & mask;
}

/**
 * @brief Set a suite of bits on a byte map
 */
int bsetbytes (__byte *table, int offset, int length)
{
  int ox = offset / 8;
  int oy = offset % 8;
  int r = 0;

  if (oy != 0 || length < 8) {
    if (length + oy < 8) {
      r |= bsetbyte(&table[ox], oy, length);
      length = 0;
    } else {
      r |= bsetbyte(&table[ox], oy, 8 - oy);
      length -= 8 - oy;
    }

    ox++;
  }

  while (length >= 8) {
    r |= table[ox];
    table[ox] = 0xff;
    ox++;
    length -= 8;
  }

  if (length > 0) {
    r |= bsetbyte(&table[ox], 0, length);
  }

  return r;
}


/* ----------------------------------------------------------------------- */
/** Unset a suite of bits on a byte map. */
int bclearbytes (__byte *table, int offset, int length)
{
  int ox = offset / 8;
  int oy = offset % 8;
  int r = 0;

  if (oy != 0 || length < 8) {
    if (length + oy < 8) {
      r |= bclearbyte(&table[ox], oy, length);
      length = 0;
    } else {
      r |= bclearbyte(&table[ox], oy, 8 - oy);
      length -= 8 - oy;
    }

    ox++;
  }

  while (length >= 8) {
    r |= ~table[ox];
    table[ox] = 0;
    ox++;
    length -= 8;
  }

  if (length > 0) {
    r |= bclearbyte(&table[ox], 0, length);
  }

  return r;
}

