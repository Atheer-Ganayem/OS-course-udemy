section .asm

extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler

global enable_interrupts
global disable_interrupts
global idt_load
global int21h
global no_interrupt
global isr80h_wrapper


enable_interrupts:
  sti
  ret

disable_interrupts:
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
  pushad

  call int21h_handler

  popad
  iret

no_interrupt:
  pushad
  call no_interrupt_handler
  popad
  iret

isr80h_wrapper:
  ; INTERRUPT FRAME START
  ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
  ; ip
  ; cs
  ; flags
  ; sp
  ; ss
  ; psuh GPRs
  pushad
  ; Interrupt frame end

  push esp ; ptr to the above
  push eax ; command code
  call isr80h_handler
  mov dword[tmp_res], eax
  add esp, 8

  ;restore GPRS
  popad
  mov eax, [tmp_res]
  iretd


section .data

tmp_res: dd 0

