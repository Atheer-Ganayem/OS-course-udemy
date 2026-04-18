#include "paging.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "kernel.h"

extern void paging_load_directory(uint32_t* directory);

int paging_set(uint32_t* directory, void* virt, uint32_t val);

static uint32_t* current_directory = 0;

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags) {
  uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
  int offset = 0;

  for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
    uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++) {
      entry[b] = (offset + b * PAGING_PAGE_SIZE) | flags;
    }
    offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
    directory[i] = (uint32_t) entry | flags | PAGING_IS_WRITEABLE;
  }

  struct paging_4gb_chunk* chunk = kzalloc(sizeof(struct paging_4gb_chunk));
  chunk->directory_entry = directory;

  return chunk;
}

void paging_switch(struct paging_4gb_chunk* chunk) {
  paging_load_directory(chunk->directory_entry);
  current_directory = chunk->directory_entry;
}

void paging_free_4gb(struct paging_4gb_chunk* chunk) {
  for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
    uint32_t entry = chunk->directory_entry[i];
    uint32_t *table = (uint32_t*)(entry & 0xFFFFF000);
    kfree(table);
  }
  kfree(chunk->directory_entry);
  kfree(chunk);
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk) {
  return chunk->directory_entry;
}

bool paging_is_aligned(void* addr) {
  return ((uint32_t)addr) % PAGING_PAGE_SIZE == 0;
}

int paging_get_indexes(void* virual_address, uint32_t* directory_index_out, uint32_t* table_index_out) {
  int res = 0;
  if (!paging_is_aligned(virual_address)) {
    res = -EINVARG;
    goto out;
  }

  *directory_index_out = ((uint32_t)virual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
  *table_index_out = ((uint32_t)virual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

  out:
  return res;
}

int paging_map(struct paging_4gb_chunk* dir, void* virt, void* phys, int flags) {
  if ((uint32_t)virt % PAGING_PAGE_SIZE || (uint32_t)phys % PAGING_PAGE_SIZE) {
    return -EINVARG;
  }

  return paging_set(dir->directory_entry, virt, (uint32_t)phys | flags);
}

int paging_map_range(struct paging_4gb_chunk* dir, void* virt, void* phys, int count, int flags) {
  int res = 0;

  for (int i = 0; i < count; i++) {
    res = paging_map(dir, virt, phys, flags);
    if (ISERR(res)) {
      break;
    }
    virt += PAGING_PAGE_SIZE;
    phys += PAGING_PAGE_SIZE;
  }

  return res;
}

int paging_map_to(struct paging_4gb_chunk* dir, void* virt, void* phys, void* phys_end, int flags) {
  int res = 0;

  if ((uint32_t)virt % PAGING_PAGE_SIZE || (uint32_t)phys % PAGING_PAGE_SIZE || (uint32_t)phys_end % PAGING_PAGE_SIZE) {
    res = -EINVARG;
    goto out;
  }

  if (phys_end < phys) {
    res = -EINVARG;
    goto out;
  }


  uint32_t total_bytes = phys_end-phys;
  int total_pages = total_bytes / PAGING_PAGE_SIZE;
  res = paging_map_range(dir, virt, phys, total_pages, flags);

out:
  return res;
}

int paging_set(uint32_t* directory, void* virt, uint32_t val) {
  if (!paging_is_aligned(virt)) {
    return -EINVARG;
  }

  uint32_t dir_index = 0, page_index = 0;
  int res = paging_get_indexes(virt, &dir_index, &page_index);
  if (res < 0) {
    return res;
  }

  uint32_t entry = directory[dir_index];
  uint32_t* table = (uint32_t*)(entry & 0xfffff000);
  table[page_index] = val;

  return 0;
}

uint32_t paging_get(uint32_t* dir, void* virt) {
  uint32_t dir_index = 0;
  uint32_t table_index = 0;

  // int res = paging_get_indexes(virt, &dir_index, &table_index);
  paging_get_indexes(virt, &dir_index, &table_index);
  uint32_t entry = dir[dir_index];
  uint32_t* table = (uint32_t*)(entry & 0xfffff000);

  return table[table_index];
}

void* paging_align_address(void* ptr) {
  if ((uint32_t)ptr % PAGING_PAGE_SIZE) {
    return (void*)((uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE));
  }
  return ptr;
}

void* paging_align_to_lower_page(void* addr) {
  uint32_t _addr = (uint32_t)addr;
  if (_addr % PAGING_PAGE_SIZE) {
    return (void*)(_addr - _addr%PAGING_PAGE_SIZE);
  } 
  return addr;
} 