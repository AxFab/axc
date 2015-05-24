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
#include <ctype.h>

const unsigned short int *__ctype_b;
const unsigned short int __ctype_ascii[128] = {
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl, // 08
  _IScntrl | _ISspace | _ISblank,
  _IScntrl | _ISspace,
  _IScntrl | _ISspace,
  _IScntrl | _ISspace,
  _IScntrl | _ISspace,
  _IScntrl,
  _IScntrl,
  _IScntrl, // 0x10
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _IScntrl,
  _ISblank | _ISspace, // 0x20 - ' '
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISdigit | _ISxdigit, // 0x30 - '0'
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISdigit | _ISxdigit,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct, // 0x40
  _ISalpha | _ISgraph | _ISupper | _ISxdigit, // 0x41 - 'A'
  _ISalpha | _ISgraph | _ISupper | _ISxdigit,
  _ISalpha | _ISgraph | _ISupper | _ISxdigit,
  _ISalpha | _ISgraph | _ISupper | _ISxdigit,
  _ISalpha | _ISgraph | _ISupper | _ISxdigit,
  _ISalpha | _ISgraph | _ISupper | _ISxdigit,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISalpha | _ISgraph | _ISupper,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISalpha | _ISgraph | _ISlower | _ISxdigit, // 0x61 - 'a'
  _ISalpha | _ISgraph | _ISlower | _ISxdigit,
  _ISalpha | _ISgraph | _ISlower | _ISxdigit,
  _ISalpha | _ISgraph | _ISlower | _ISxdigit,
  _ISalpha | _ISgraph | _ISlower | _ISxdigit,
  _ISalpha | _ISgraph | _ISlower | _ISxdigit,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISalpha | _ISgraph | _ISlower,
  _ISgraph | _ISpunct, // 0x7b
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _ISgraph | _ISpunct,
  _IScntrl, // 0x7f
};


const unsigned short int **__ctype_b_loc (void)
{
  __ctype_b = __ctype_ascii;
  return &__ctype_b;
}

