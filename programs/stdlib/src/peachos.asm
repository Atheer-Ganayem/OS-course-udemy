[BITS 32]

section .asm 

global print:function
global peachos_getkey:function
global peachos_malloc:function
global peachos_free:function
global peachos_putchar:function
global peachos_process_load_start:function

;void print(const char* message)
print:
  push ebp
  mov ebp, esp

  push dword[ebp+8]
  mov eax, 1 ; cmd print
  int 0x80

  add esp, 4

  pop ebp
  ret

;int getkey()
peachos_getkey:
  push ebp
  mov ebp, esp

  mov eax, 2
  int 0x80

  pop ebp
  ret

; void putchar(char c)
peachos_putchar:
  push ebp
  mov ebp, esp

  mov eax, 3
  push dword [ebp+8]
  int 0x80

  add esp, 4
  pop ebp
  ret

;void* peachos_malloc(size_t size)
peachos_malloc:
  push ebp
  mov ebp, esp

  mov eax, 4 ; cmd malloc
  push dword[ebp+8]
  int 0x80

  add esp, 4
  pop ebp
  ret

;void* peachos_free(void* ptr)
peachos_free:
  push ebp
  mov ebp, esp

  mov eax, 5 ; cmd free
  push dword[ebp+8]
  int 0x80

  add esp, 4
  pop ebp
  ret

; void peachos_process_load_start(const char* filename) 
peachos_process_load_start:
  push ebp
  mov ebp, esp

  mov eax, 6
  push dword[ebp+8]
  int 0x80

  add esp, 4
  pop ebp
  ret