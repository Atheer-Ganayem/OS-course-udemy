#include "peachos.h"

int main(int argc, char** argv) {
  print("Hello from blank\n");

  while(1){
    int c = getkey();
    if (c) {
      print("Key was pressed!\n");
    }
  }
  return 0;
}