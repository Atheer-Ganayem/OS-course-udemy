#include "process.h"
#include "task/task.h"
#include "config.h"
#include "status.h"
#include "task/process.h"
#include "string/string.h"
#include "kernel.h"

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

void* isr80h_command7_invoke_system_command(struct interupt_frame* frame) {
  struct command_argument* args = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));
  if (!args || strlen(args[0].argument) == 0) {
    return ERROR(-EINVARG);
  }

  struct command_argument* root_cmd_arg = &args[0];
  const char* program_name = root_cmd_arg->argument;

  char path[PEACHOS_MAX_PATH];
  strcpy(path, "0:/");
  strncpy(path + 3, program_name, sizeof(path)-3);

  struct process* proc = NULL;
  int res = process_load_switch(path, &proc);
  if (ISERR(res)) {
    return ERROR(res);
  }

  res = process_inject_arguments(proc, root_cmd_arg);
  if (ISERR(res)) {
    return ERROR(res);
  }
  task_switch(proc->task);
  task_return(&proc->task->registers);

  return 0;
}

void* isr80h_command6_get_program_arguments(struct interupt_frame* frame) {
  struct process* proc = task_current()->process;
  struct process_arguments* arguments = task_virtual_address_to_physical(task_current(), task_get_stack_item(task_current(), 0));

  process_get_arguments(proc, &arguments->argc, &arguments->argv);

  return 0;
}