#include "peachos.h"
#include "stdlib.h"

int main(int argc, char** argv) {
  print("Hello from blank\n");

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