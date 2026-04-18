#include "process.h"
#include "memory/memory.h"
#include "status.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "fs/file.h"
#include "string/string.h"
#include "memory/paging/paging.h"
#include "loader/formats/elfloader.h"

struct process* current_process = NULL;
static struct process* processes[PEACHOS_MAX_PROCESSES] = {};

static void process_init(struct process* proc) {
  memset(proc, 0x00, sizeof(struct process));
}

struct process* process_current() {
  return current_process;
}

struct process* process_get(int proc_id) {
  if (proc_id < 0 || proc_id >= PEACHOS_MAX_PROCESSES) {
    return NULL;
  }

  return processes[proc_id];
}

int process_switch(struct process* proc) {
  current_process = proc;
  return PEACHOS_ALL_OK;
}

static int process_load_binary(const char* filename, struct process* proc) {
  int res = 0;

  int fd = fopen(filename, "r");
  if (!fd) {
    res = -EIO;
    goto out;
  }

  struct file_stat stat;
  res = fstat(fd, &stat);
  if (res != PEACHOS_ALL_OK) {
    goto out;
  }

  void* program_data_ptr = kzalloc(stat.file_size);
  if (!program_data_ptr) {
    res = -ENOMEM;
    goto out;
  }

  if (fread(program_data_ptr, stat.file_size, 1, fd) != 1) {
    res = -EIO;
    goto out;
  }

  proc->ptr = program_data_ptr;
  proc->size = stat.file_size;
  proc->filetype = PROCESS_FILE_TYPE_BIN;
  
out:
  fclose(fd);
  return res;
}

static int process_load_elf(const char* filename, struct process* proc) {
  int res = 0;

  struct elf_file* file = NULL;

  res = elf_load(filename, &file);
  if (ISERR(res)) {
    return res;
  }

  proc->filetype = PROCESS_FILE_TYPE_ELF;
  proc->elf_file = file;

  return res;
}

static int process_load_data(const char* filename, struct process* proc) {
  int res = 0;
  res = process_load_elf(filename, proc);
  if (res == -EINFORMAT) {
    res = process_load_binary(filename, proc);
  }

  return res;
}

int process_map_binary(struct process* proc) {
  int res = 0;

  struct paging_4gb_chunk* dir = proc->task->page_directory;
  int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE;
  res = paging_map_to(dir, (void*) PEACHOS_PROGRAM_VIRTUAL_ADDRESS, proc->ptr, paging_align_address(proc->ptr + proc->size), flags);

  return res;
}

static int process_map_elf(struct process* proc) {
  int res = 0;

  struct paging_4gb_chunk* dir = proc->task->page_directory;
  struct elf_file* file = proc->elf_file;
  struct elf_header* header = elf_header(file);
  struct elf32_phdr* phdrs = elf_pheader(header);

  for (int i = 0; i < header->e_phnum; i++) {
    struct elf32_phdr* phdr = &phdrs[i];
    void* phdr_phys_addr = elf_phdr_phys_address(file, phdr);
    int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
    if (phdr->p_flags & PF_W) {
      flags |= PAGING_IS_WRITEABLE;
    }
    void* base_virt_addr = paging_align_to_lower_page((void*)phdr->p_vaddr);
    void* base_phys_addr = paging_align_to_lower_page(phdr_phys_addr);
    res = paging_map_to(dir, base_virt_addr, base_phys_addr, paging_align_address(phdr_phys_addr+phdr->p_filesz), flags);
    if (ISERR(res)) {
      break;
    }
  }


  return res;
}

int process_map_memory(struct process* proc) {
  int res = process_map_binary(proc);

  switch (proc->filetype) {
    case PROCESS_FILE_TYPE_ELF:
      res = process_map_elf(proc);
      break;
    case PROCESS_FILE_TYPE_BIN:
      res = process_map_binary(proc);
      break;
    default:
      panic("process_map_memory: Invalid PROCESS_FILE_TYPE.\n");
  }

  if (ISERR(res)) {
    return res;
  }

  // map the stack
  int flags = PAGING_IS_PRESENT | PAGING_IS_WRITEABLE | PAGING_ACCESS_FROM_ALL;
  res = paging_map_to(proc->task->page_directory, (void*)PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, proc->stack, paging_align_address(proc->stack + PEACHOS_USER_PROGRAM_STACK_SIZE), flags);

  return res;
}

int process_load_for_slot(const char* filename, struct process** process, int process_slot) {
  int res = 0;

  struct task* task = NULL;
  struct process* proc;
  void* program_stack_ptr = NULL;

  if (process_get(process_slot) != NULL) {
    res = -EISTKN;
    goto out;
  }

  proc = kzalloc(sizeof(struct process));
  if (!proc) {
    res = -ENOMEM;
    goto out;
  }

  process_init(proc);
  res = process_load_data(filename, proc);
  if (ISERR(res)) {
    goto out;
  }

  program_stack_ptr = kzalloc(PEACHOS_USER_PROGRAM_STACK_SIZE);
  if (!program_stack_ptr) {
    res = -ENOMEM;
    goto out;
  }

  strncpy(proc->filename, filename, sizeof(proc->filename));
  proc->stack = program_stack_ptr;
  proc->id = process_slot;

  // create task
  task = task_new(proc);
  if (ERROR_I(task) == 0) {
    res = ERROR_I(task);
    goto out;
  }

  proc-> task = task;

  res = process_map_memory(proc);
  if (res < 0) {
    goto out;
  }

  *process = proc;

  // add the proc to the array
  processes[process_slot] = proc;

out:
  if (ISERR(res)) {
    if (proc && proc->task) {
      task_free(proc->task);
    }
    // later: free proc, stack, data, etc...
  }
  return res;
}

int process_get_free_slot() {
  for (int i = 0; i < PEACHOS_MAX_PROCESSES; i++) {
    if (!processes[i]) {
      return i;
    }
  }

  return EISTKN;
}

int process_load(const char* filename, struct process** process) {
  int res = 0;
  int process_slot = process_get_free_slot();
  if (process_slot < 0) {
    res = -ENOMEM;
    goto out;
  }

  res = process_load_for_slot(filename, process, process_slot);

out:
  return res;
}

int process_load_switch(const char* filename, struct process** proc) {
  int res = process_load(filename, proc);
  if (res == PEACHOS_ALL_OK) {
    process_switch(*proc);
  }
  return res;
}