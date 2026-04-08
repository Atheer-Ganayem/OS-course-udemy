section .asm

extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global enable_interrupts
global disable_interrupts
global idt_load
global no_interrupt
global isr80h_wrapper
global interrupt_pointer_table


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

no_interrupt:
  pushad
  call no_interrupt_handler
  popad
  iret

%macro interrupt 1
  global int%1
  int%1:
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
    push esp
    push dword %1
    call interrupt_handler
    add esp, 8
    popad
    iret
%endmacro

%assign i 0
%rep 512
  interrupt i
%assign i  i+1
%endrep


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

%macro interrupt_array_entry 1
  dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
  interrupt_array_entry i
%assign i i+1
%endrep
