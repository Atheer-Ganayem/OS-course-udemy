#include "peachos.h"

int peachos_getkey_block() {
  int val;
  do {
    val = peachos_getkey();
  } while (!val);
  
  return val;
}

void peachos_terminal_readline(char* out, size_t max, bool output_while_typing) {
  int i = 0;
  
  // saving last byte for null terminator 
  for (i = 0; i < max-1; i++) {
    int c = peachos_getkey_block();

    if (c == 13) {
      break;
    }

    if (c == 0x08 && i == 0) {
      i--;
    } else if (c == 0x08 && i > 0) { // Backspace
      if (output_while_typing) peachos_putchar(c);
      // decreasing i twice because after before we loop again we are gonna i++
      out[--i] = 0x00;
      --i;
    } else if (c != 0x08) {
      out[i] = c;
      if (output_while_typing) peachos_putchar(c);
    }
  }

  out[i] = 0x00;
}