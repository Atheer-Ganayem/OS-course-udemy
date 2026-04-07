#include "io.h"
#include "task/task.h"
#include "config.h"
#include "memory/heap/kheap.h"
#include "kernel.h"

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