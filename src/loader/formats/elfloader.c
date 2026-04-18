#include "elfloader.h"
#include "elf.h"
#include "fs/file.h"
#include "status.h"
#include <stdbool.h>
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "string/string.h"
#include "memory/paging/paging.h"
#include "kernel.h"
#include "config.h"


const char elf_signature[] = {0x7F, 'E', 'L', 'F'};

static bool elf_valid_signature(void* buf) {
  return memcmp(buf, (void*)elf_signature, sizeof(elf_signature)) == 0;
}

static bool elf_valid_class(struct elf_header* header) {
  // we only support 32 binaries.
  return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

static bool elf_valid_encoding(struct elf_header* header) {
  return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

static bool elf_is_executable(struct elf_header* header) {
  return header->e_type == ET_EXEC && header->e_entry >= PEACHOS_PROGRAM_VIRTUAL_ADDRESS;
}

static bool elf_has_program_header(struct elf_header* header) {
  return header->e_phoff != 0;
}

void* elf_memory(struct elf_file* file) {
  return file->elf_memory;
}

struct elf_header* elf_header(struct elf_file* file) {
  return file->elf_memory;
}

struct elf32_shdr* elf_sheader(struct elf_header* header) {
  return (struct elf32_shdr*)((int)header+header->e_shoff);
}

struct elf32_phdr* elf_pheader(struct elf_header* header) {
  if(header->e_phoff == 0) {
    return NULL;
  }

  return (struct elf32_phdr*)((int)header + header->e_phoff);
}

struct elf32_phdr* elf_program_header(struct elf_header* header, int index) {
  return &elf_pheader(header)[index];
}

struct elf32_shdr* elf_section(struct elf_header* header, int index) {
  return &elf_sheader(header)[index];
}

char* elf_str_table(struct elf_header* header) {
  return (char*) header + elf_section(header, header->e_shstrndx)->sh_offset;
}

void* elf_virtual_base(struct elf_file* file) {
  return file->virtual_base_address;
}

void* elf_virtual_end(struct elf_file* file) {
  return file->virtual_end_address;
}

void* elf_phys_base(struct elf_file* file) {
  return file->physical_base_address;
}

void* elf_phys_end(struct elf_file* file) {
  return file->physical_end_address;
}

int elf_validate_loaded(struct elf_header* header) {
  return (elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header) && elf_has_program_header(header))
    ? PEACHOS_ALL_OK : -EINFORMAT;
}

void* elf_phdr_phys_address(struct elf_file* file, struct elf32_phdr* phdr) {
  return elf_memory(file) + phdr->p_offset;
}

int elf_process_phdr_pt_load(struct elf_file* file, struct elf32_phdr* phdr) {
  int res = 0;

  if (file->virtual_base_address >= (void*)phdr->p_vaddr || file->virtual_base_address == 0x00) {
    file->virtual_base_address = (void*)phdr->p_vaddr;
    file->physical_base_address = file->elf_memory + phdr->p_offset;
  }

  unsigned int end_virt_addr = phdr->p_vaddr + phdr->p_filesz;
  if (file->virtual_end_address <= (void*) end_virt_addr || file->virtual_end_address == 0x00) {
    file->virtual_end_address = (void*) end_virt_addr;
    file->physical_end_address = file->elf_memory + phdr->p_offset + phdr->p_filesz;
  }

out:
  return res;
}

int elf_process_pheader(struct elf_file* file, struct elf32_phdr* phdr) {
  int res = 0;

  switch (phdr->p_type)
  {
  case PT_LOAD:
    res = elf_process_phdr_pt_load(file, phdr);
    break;
  
  default:
    res = -EINVELFTYPE;
    break;
  }

  return res;
}

int elf_process_pheaders(struct elf_file* file) {
  int res = 0;

  struct elf_header* header = elf_header(file);

  for (int i = 0; i < header->e_phnum; i++) {
    struct elf32_phdr* phdr = elf_program_header(header, i);
    res = elf_process_pheader(file, phdr);
    if (ISERR(res)) {
      break;
    }
  }

out:
  return res;
}

int elf_process_loaded(struct elf_file* file) {
  int res = 0;

  struct elf_header* header = elf_header(file);
  
  res = elf_validate_loaded(header);
  if (ISERR(res)) {
    goto out;
  }

  res = elf_process_pheaders(file);
  if (ISERR(res)) {
    goto out;
  }

out:
  return res;
}

int elf_load(const char* filename, struct elf_file** file_out) {
  int res = 0;

  struct elf_file* elf_file = kzalloc(sizeof(struct elf_file));
  if (!elf_file) {
    return -ENOMEM;
  }

  int fd = fopen(filename, "r");
  if (ISERR(fd)) {
    res = fd;
    goto out;
  }

  struct file_stat stat;
  res = fstat(fd, &stat);
  if (ISERR(res)) {
    goto out;
  }

  elf_file->elf_memory = kzalloc(stat.file_size);
  if (!elf_file->elf_memory) {
    res = -ENOMEM;
    goto out;
  }

  res = fread(elf_file->elf_memory, stat.file_size, 1, fd);
  if (ISERR(res)) {
    goto clean;
  }

  res = elf_process_loaded(elf_file);
  if (ISERR(res)) {
    goto clean;
  }

  *file_out = elf_file;
  return 0;

clean:
  kfree(elf_file->elf_memory);

out:
  kfree(elf_file);
  fclose(fd);
  return res;
}

void elf_close(struct elf_file* file) {
  if (!file) {
    return;
  }

  kfree(file->elf_memory);
  kfree(file);
}