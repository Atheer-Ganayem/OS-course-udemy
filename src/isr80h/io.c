#include "io.h"
#include "task/task.h"
#include "config.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "keyboard/keyboard.h"

void* isr80h_command1_print(struct interupt_frame* frame) {
  void* user_space_msg_buf = task_get_stack_item(task_current(), 0);
  char buf[1024] = {0};
  int res = copy_string_from_task(task_current(), user_space_msg_buf, buf, sizeof(buf));
  if (ISERR(res)) {
    goto out;
  }

  print(buf);

out:
  return 0;
}

void* isr80h_command2_getkey(struct interupt_frame* frame) {
  char c = keyboard_pop();
  return (void*)(int)c;
}

void* isr80h_command3_putchar(struct interupt_frame* frame) {
  char c = (char)(int)task_get_stack_item(task_current(), 0);
  terminal_write_char(c, 15);
  
  return 0;
}