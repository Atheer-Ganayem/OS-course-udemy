#include "classic.h"
#include "keyboard.h"
#include "io/io.h"
#include "stddef.h"
#include "kernel.h"
#include "idt/idt.h"

static uint8_t keyboard_scan_set_one[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`', 
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'
};

int classic_keyboard_init();
void classic_keyboard_handle_interupt();

struct keyboard classic_keyboard = {
  .name = "classic",
  .init = classic_keyboard_init,
};

int classic_keyboard_init() {
  idt_register_interrupt_callback(0x21, classic_keyboard_handle_interupt);
  outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
  return 0;
}

struct keyboard* classic_init() {
  return &classic_keyboard;
}

uint8_t classic_keyboard_scancode_to_char(uint8_t scancode) {
  size_t set_one_size = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);
  if (scancode > set_one_size) {
    return 0;
  }

  return keyboard_scan_set_one[scancode];
}

void classic_keyboard_handle_interupt() {
  kernel_page();
  uint8_t scancode = 0;
  scancode = insb(PS2_INPUT_PORT);
  insb(PS2_INPUT_PORT);

  if (scancode & CLASSIC_KEYBOARD_KEY_RELEASED) {
    return;
  }

  uint8_t c = classic_keyboard_scancode_to_char(scancode);
  if (c != 0) {
    keyboard_push(c);
  }

  task_page();
}