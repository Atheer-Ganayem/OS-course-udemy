#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "config.h"
#include "task.h"

struct process {
  uint16_t id;
  char filename[PEACHOS_MAX_PATH];

  struct task* task;
  void* allocations[PEACHOS_MAX_PROGRAM_ALLOCATIONS];

  // The phisical pointer to the process memory.
  void* ptr;
  // The phisical pointer to the stack
  void* stack;

  // The size of data pointed to by "ptr";
  uint32_t size;
};

int process_load(const char* filename, struct process** process);

#endif