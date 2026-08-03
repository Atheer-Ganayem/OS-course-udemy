#include "process.h"
#include "task/task.h"
#include "config.h"
#include "status.h"
#include "task/process.h"
#include "string/string.h"

void* isr80h_command6_process_load_start(struct interupt_frame* frame) {
  void* filename_user_ptr = task_get_stack_item(task_current(), 0);
  char filename[PEACHOS_MAX_PATH];
  strcpy(filename, "0:/");

  int res = copy_string_from_task(task_current(), filename_user_ptr, filename+3, sizeof(filename)-3);
  if (res < 0) {
    goto out;
  }

  struct process* process = NULL;
  res = process_load_switch(filename, &process);
  if (res < 0) {
    goto out;
  }

  task_switch(process->task);

  task_return(&process->task->registers);

out:
  return (void*) res;
}