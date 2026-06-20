#include "peachos.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv) {
  print("Hello from blank\n");
  print(itoa(8763));
  putchar('\n');
  putchar('Z');
  putchar('\n');
  printf("%s: %i", "The number is", 67);
  putchar('\n');
  
  void* ptr = peachos_malloc(512);
  if (ptr) {
    print("allocated mem");
  }
  free(ptr);

  while(1){
    int c = getkey();
    if (c) {
      print("Key was pressed!\n");
    }
  }
  return 0;
}