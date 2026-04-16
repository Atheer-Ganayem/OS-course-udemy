#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>

#include "elf.h"
#include "config.h"

struct elf_file {
  char filename[PEACHOS_MAX_PATH];
  int in_memory_size;
  
  // the phisical addr where the elf file is loaded at.
  void* elf_memory;

  // virtual base addr of this binary.
  void* virtual_base_address;

  void* virtual_end_address;

  void* physical_base_address;

  void* physical_end_address;
};

#endif