#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "config.h"
#include "task.h"

typedef unsigned char PROCESS_FILE_TYPE;
#define PROCESS_FILE_TYPE_ELF 0
#define PROCESS_FILE_TYPE_BIN 1

struct process {
  uint16_t id;
  char filename[PEACHOS_MAX_PATH];
  PROCESS_FILE_TYPE filetype;

  struct task* task;
  void* allocations[PEACHOS_MAX_PROGRAM_ALLOCATIONS];

  union {
    // The phisical pointer to the process memory.
    void* ptr;
    struct elf_file* elf_file;
  };

  // The phisical pointer to the stack
  void* stack;

  // The size of data pointed to by "ptr";
  uint32_t size;

  struct keyboard_buffer {
    char buf[PEACHOS_KEYBOARD_BUFFER_SIZE];
    int tail;
    int head;
  } keyboard;
};

struct process* process_current();
int process_load(const char* filename, struct process** process);
int process_load_switch(const char* filename, struct process** proc);
int process_switch(struct process* proc);
void* process_malloc(struct process* proc, size_t size);
void* process_free(struct process* proc, void* ptr);

#endif