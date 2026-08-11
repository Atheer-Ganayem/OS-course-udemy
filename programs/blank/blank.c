#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char** argv) {
  printf("argc: %i\n", argc);
  for (int i = 0; i < argc; i++) {
    print(argv[i]);
    putchar('\n');
  }

  while(1){
  }
  return 0;
}