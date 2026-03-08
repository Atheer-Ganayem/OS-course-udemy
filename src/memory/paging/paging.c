#include "paging.h"
#include "memory/heap/kheap.h"
#include "status.h"

extern void paging_load_directory(uint32_t* directory);

static uint32_t* current_directory = 0;

struct page_4gb_chunck* paging_new_4gb(uint8_t flags) {
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

  struct page_4gb_chunck* chunck = kzalloc(sizeof(struct page_4gb_chunck));
  chunck->directory_entry = directory;

  return chunck;
}

void paging_swtich(uint32_t* directory) {
  paging_load_directory(directory);
  current_directory = directory;
}

uint32_t* paging_4gb_chunck_get_directory(struct page_4gb_chunck* chunck) {
  return chunck->directory_entry;
}

bool paging_is_aligned(void* addr) {
  return ((uint32_t)addr) % PAGING_PAGE_SIZE == 0;
}

int paging_get_indexes(void* virual_address, uint32_t* directory_index_out, uint32_t* table_index_out) {
  int res = 0;
  if (!paging_is_aligned(virual_address)) {
    res = -ENVARG;
    goto out;
  }

  *directory_index_out = ((uint32_t)virual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
  *table_index_out = ((uint32_t)virual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

  out:
  return res;
}

int paging_set(uint32_t* directory, void* virt, uint32_t val) {
  if (!paging_is_aligned(virt)) {
    return -ENVARG;
  }

  uint32_t dir_index = 0, page_index = 0;
  int res = paging_get_indexes(virt, &dir_index, &page_index);
  if (res < 0) {
    return res;
  }

  uint32_t entry = directory[dir_index];
  uint32_t* table = (uint32_t*)(entry& 0xfffff000);
  table[page_index] = val;

  return 0;
}