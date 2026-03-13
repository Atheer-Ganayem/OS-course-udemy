#include "file.h"
#include "config.h"
#include "stddef.h"
#include "memory/memory.h"
#include "status.h"
#include "memory/heap/kheap.h"
#include "kernel.h"
#include "fat/fat16.h"

struct filesystem* filesystems[PEACHOS_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[PEACHOS_MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_file_system() {
  int i = 0;
  for (i = 0; i < PEACHOS_MAX_FILESYSTEMS; i++) {
    if (filesystems[i] == NULL) {
      return &filesystems[i];
    }
  }

  return NULL;
}

void fs_insert_filesystem(struct filesystem* filesystem) {
  // if (filesystem == NULL) {
    //   // PANIC. later
    // }
  struct filesystem** fs = fs_get_free_file_system();
  if (!fs) {
    // panic. later
    print("Problem inserting filesystem"); 
    while(1){};
  }

  *fs = filesystem;
}

static void fs_static_load() {
  fs_insert_filesystem(fat16_init());
}

void fs_load() {
  memset(filesystems, 0, sizeof(filesystems));
  fs_static_load();
}

void fs_init() {
  memset(file_descriptors, 0, sizeof(file_descriptors));
  fs_load();
}

static int file_new_descriptor(struct file_descriptor** desc_out) {
  int res = -ENOMEM;
  for (int i = 0; i < PEACHOS_MAX_FILE_DESCRIPTORS; i++) {
    if (file_descriptors[i] == NULL) {
      struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
      
      // descriptors start at 1
      desc->index = i + 1;
      file_descriptors[i] = desc;
      *desc_out = desc;
      res = 0;
      break;
    }
  }

  return res;
}

static struct file_descriptor* file_get_descriptor(int fd) {
  if (fd <= 0 || fd >= PEACHOS_MAX_FILE_DESCRIPTORS) {
    return NULL;
  }

  return file_descriptors[fd-1];
}

struct filesystem* fs_resolve(struct disk* disk) {
  struct filesystem* fs = NULL;
  for (int i = 0; i < PEACHOS_MAX_FILESYSTEMS; i++) {
    if (filesystems[i] && filesystems[i]->resolve(disk) == 0) {
      fs = filesystems[i];
      break;
    }
  }

  return fs;
}

int fopen(const char *filename, const char* mode) {
  return -EIO;
}