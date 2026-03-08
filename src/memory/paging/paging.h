#ifndef PAGING_H
#define PAGIN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGING_CHACHE_DISABLED 0b00010000
#define PAGING_WRITE_THROUGH   0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITEABLE    0b00000010
#define PAGING_IS_PRESENT      0b00000001


#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

struct page_4gb_chunck {
  uint32_t* directory_entry;
};
//  __attribute__((packed));

struct page_4gb_chunck* paging_new_4gb(uint8_t flags);
void paging_swtich(uint32_t* directory);
void enable_paging();
bool paging_is_aligned(void* addr);
uint32_t* paging_4gb_chunck_get_directory(struct page_4gb_chunck* chunck);
int paging_set(uint32_t* directory, void* virt, uint32_t val);
#endif