#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv) {
  print("\nHello from blank\n");
  printf("%s: %i", "The number is\n", 67);
  
  void* ptr = malloc(512);
  if (ptr) {
    print("allocated mem\n");
  }
  free(ptr);

  char line[512];
  peachos_terminal_readline(line, sizeof(line), true);
  print("\nline entered!!\n");

  while(1){
  }
  return 0;
}