#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv) {
  print("Hello from blank\n");
  print(itoa(8763));
  putchar('\n');
  putchar('Z');
  putchar('\n');
  printf("%s: %i", "The number is\n", 67);
  putchar('\n');
  
  void* ptr = malloc(512);
  if (ptr) {
    print("allocated mem\n");
  }
  free(ptr);

  int c = peachos_getkey_block();
  if (c) {
    print("Key was pressed!\n");
  }

  char line[512];
  peachos_terminal_readline(line, sizeof(line), true);
  print("\nline entered!!\n");

  while(1){
  }
  return 0;
}