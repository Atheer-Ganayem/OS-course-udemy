#include "keyboard.h"
#include "stddef.h"
#include "status.h"
#include "kernel.h"
#include "task/task.h"
#include "classic.h"

static struct keyboard* keyboard_list_head = NULL;
static struct keyboard* keyboard_list_last = NULL;

int keyboard_insert(struct keyboard* kb);

void keyboard_init() {
  keyboard_insert(classic_init());
}

int keyboard_insert(struct keyboard* kb) {
  if (!kb->init) {
    return -EINVARG;
  }

  if (!keyboard_list_head) {
    keyboard_list_head = kb;
    keyboard_list_last = kb;
    goto init_out;
  }

  keyboard_list_last->next = kb;
  keyboard_list_last = kb;

init_out:
  return kb->init();
}

int keyboard_get_tail_index(struct process* proc) {
  return proc->keyboard.tail % sizeof(proc->keyboard.buf);
}

void keyboard_backspace(struct process* proc) {
  proc->keyboard.tail--;
  int real_index = keyboard_get_tail_index(proc);
  proc->keyboard.buf[real_index] = 0x00;
  // something feels wrong....
} 

void keyboard_push(char c) {
  if (!c) return;

  struct process* proc = process_current();
  if (!proc) {
    return;
  }

  int real_index = keyboard_get_tail_index(proc);
  proc->keyboard.buf[real_index] = c;
  proc->keyboard.tail++;
}

char keyboard_pop() {
  if (!task_current()) {
    return 0x00;
  }

  struct process* proc = task_current()->process;
  int real_index = proc->keyboard.head % sizeof(proc->keyboard.buf);
  char c =  proc->keyboard.buf[real_index];

  if (c == 0x00) {
    return 0x00;
  }

  proc->keyboard.buf[real_index] = 0;
  proc->keyboard.head++;
  
  return c;
}