[BITS 32]

section .asm

global restore_general_purpose_registers
global task_return
global user_registers

task_return:
  mov ebp, esp
  ; PUSH THE DATA SEGMENT (SS WILL BE FINE)
  ; PUSH THE STACK ADDRESS
  ; PUSH THE FLAGS
  ; PUSH THE CODE SEGMENT
  ; PUSH IP

  ; accessing regs
  mov ebx, [ebp+4]
  ; pushing ss
  push dword [ebx+44]
  ; pushing esp
  push dword [ebx+40]
  
  ; pushing flags
  pushf
  pop eax
  or eax, 0x200
  push eax

  ; pushing cs
  push dword [ebx+32]
  
  ; push ip
  push dword [ebx+28]

  ; setup some segment registers
  mov ax, [ebx+44]
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  push dword [ebp+4]
  call restore_general_purpose_registers
  add esp, 4
  
  iretd

; void restore_general_purpos_registers(struct registers* regs);
restore_general_purpose_registers:
  push ebp
  mov ebp, esp
  mov ebx, [ebp+8]
  mov edi, [ebx]
  mov esi, [ebx+4]
  mov ebp, [ebx+8]
  mov edx, [ebx+16]
  mov ecx, [ebx+20]
  mov eax, [ebx+24]
  mov ebx, [ebx+12]
  pop ebp
  ret
  ; push ebp
  ; mov ebp, esp
  ; mov ebx, [ebp+8]
  ; mov edi, [ebx]
  ; mov esi, [ebx+4]
  ; mov ebp, [ebx+8]
  ; mov edx, [ebx+16]
  ; mov ecx, [ebx+20]
  ; mov eax, [ebx+24]
  ; mov ebx, [ebx+12]
  ; add esp, 4
  ; ret

user_registers:
  mov ax, 0x23 ; 0x20 (user data segment) in the GDT with RPL 3
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs ,ax
  ret