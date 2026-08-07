#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char** argv) {
  print("\nHello from blank\n");

  char* ptr = malloc(20);
  strcpy(ptr, "Hello world!");
  print(ptr);
  free(ptr);

  ptr[0] = 'T';
  print(ptr);

  while(1){
  }
  return 0;
}