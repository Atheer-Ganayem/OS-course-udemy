[BITS 32]

section .asm

global _start

_start:
  call keyboard_test

keyboard_test:
  call getkey
  push eax
  mov eax, 3
  int 0x80
  add esp, 4

  cmp eax, 0x00
  je keyboard_test

  call print_err

  jmp $

getkey:
  mov eax, 2
  int 0x80
  cmp eax, 0x00
  je getkey
  ret

print_err:
  push error_msg
  mov eax, 1
  int 0x80
  add esp, 4
  ret

section .data
message: db `Hello world from user app!`, 0x0a, 0
error_msg: db `An Error occurred`, 0x0a, 0