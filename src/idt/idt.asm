section .asm

extern int21h_handler
extern no_interrupt_handler

global enable_interupts
global disable_interupts
global idt_load
global int21h
global no_interrupt


enable_interupts:
  sti
  ret

disable_interupts:
  cli
  ret

idt_load:
  push ebp
  mov ebp, esp

  mov ebx, [ebp+8]
  lidt [ebx]

  pop ebp
  ret

int21h:
  cli
  pushad

  call int21h_handler

  popad
  iret
  sti

no_interrupt:
  cli
  pushad
  call no_interrupt_handler
  popad
  iret
  sti