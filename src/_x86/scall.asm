;
;      This file is part of the SmokeOS project.
;  Copyright (C) 2015  <Fabien Bavent>
;
;  This program is free software: you can redistribute it and/or modify
;  it under the terms of the GNU Lesser General Public License as
;  published by the Free Software Foundation, either version 3 of the
;  License, or (at your option) any later version.
;
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU Lesser General Public License for more details.
;
;  You should have received a copy of the GNU Lesser General Public License
;  along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
;   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

global _start
global __syscall
extern __axc_start

_start:
  call __axc_start
  jmp $

__syscall:
  push ebp
  mov ebp, esp
  pusha
  mov eax, [ebp + 8]
  mov ecx, [ebp + 12]
  mov edx, [ebp + 16]
  mov ebx, [ebp + 20]
  mov esi, [ebp + 24]
  mov edi, [ebp + 28]
  int 0x30
  mov [ebp + 8], eax
  mov [ebp + 12], edx
  popa
  mov eax, [ebp + 8]
  leave
  ret

; __syscall:
;   push ebp
;   mov ebp, esp
;   pusha
;   mov eax, [ebp + 8]
;   mov ebx, [ebp + 12]
;   mov ecx, [ebp + 16]
;   mov edx, [ebp + 20]
;   mov esi, [ebp + 24]
;   mov edi, [ebp + 24]
;   int 0x30
;   mov [ebp + 8], eax
;   mov [ebp + 12], edx
;   popa
;   mov eax, [ebp + 8]
;   leave
;   ret
