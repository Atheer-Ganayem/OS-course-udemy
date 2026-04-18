#include <kernel.h>
#include "stdint.h"
#include <stddef.h>
#include "idt/idt.h"
#include "isr80h/isr80h.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "string/string.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/memory.h"
#include "task/tss.h"
#include "task/process.h"
#include "status.h"
#include "keyboard/keyboard.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0, terminal_col = 0;

uint16_t terminal_make_char(char c, char color) {
  return (color << 8) | c;
}

void terminal_put_char(int x, int y, char c, char color) {
  video_mem[y*VGA_WIDTH + x] = terminal_make_char(c, color);
}

void terminal_backspace() {
  if (terminal_col == 0 && terminal_row == 0)
    return;

  if (terminal_col > 0) {
    terminal_col--;
  } else {
    terminal_col = VGA_WIDTH-1;
    terminal_row--;
  }

  terminal_put_char(terminal_col, terminal_row, 0x00, 15);
}

void terminal_write_char(char c, char color) {
  if (c == '\n') {
    terminal_row++;
    terminal_col = 0;
    return;
  }

  if (c == 0x08) { //backspace
    terminal_backspace();
    return;
  }

  terminal_put_char(terminal_col, terminal_row, c, color);
  terminal_col++;
  if (terminal_col >= VGA_WIDTH) {
    terminal_col = 0;
    terminal_row++;
  }
}

void terminal_intialize() {
  video_mem = (uint16_t*)(0xB8000);
  for (int y = 0; y < VGA_HEIGHT; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      terminal_put_char(x, y, ' ', 0);
    }
  }
}

void print(const char* str) {
  size_t len = strlen(str);
  for (int i = 0; i < len; i++) {
    terminal_write_char(str[i], 15);
  }
}

static struct paging_4gb_chunk* kernel_4gb_chunk = 0;

void panic(const char* msg) {
  print(msg);
  while(1){}
}

void kernel_page() {
  kernel_registers();
  paging_switch(kernel_4gb_chunk);
}

struct tss tss;

struct gdt gdt_real [PEACHOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[PEACHOS_TOTAL_GDT_SEGMENTS] = {
  {.base = 0x00, .limit = 0x00, .type = 0x00},                  // NULL segment
  {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x9A},            // Kernel code segment
  {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92},            // Kernel data segment
  {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF8},            // User code segment
  {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF2},            // User data segment
  {.base = (uint32_t)(&tss), .limit = sizeof(tss), .type = 0xE9}, // TSS segment
};

void test_file();

void kernel_main() {
  terminal_intialize();
  // print("Hello world!\nI'm Atheer :)\n");

  memset(gdt_real, 0x00, sizeof(gdt_real));
  gdt_structured_to_gdt(gdt_real, gdt_structured, PEACHOS_TOTAL_GDT_SEGMENTS);
  gdt_load(gdt_real, sizeof(gdt_real));

  kheap_init();

  fs_init();

  disk_search_and_init();

  idt_init();

  // setup tss
  memset(&tss, 0x00, sizeof(tss));
  tss.esp0 = 0x600000;
  tss.ss0 = KERNEL_DATA_SELECTOR;
  tss_load(0x28);

  kernel_4gb_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
  paging_switch(kernel_4gb_chunk);
  enable_paging();

  isr80h_register_commands();

  keyboard_init();

  // initializaion finished

  struct process* proc = NULL;
  int res = process_load_switch("0:/blank.elf", &proc);
  if (res != PEACHOS_ALL_OK) {
    panic("Failed to load blank.elf\n");
  }

  task_run_first_ever_task();

  while(1){}
}

void test_file() {
  int fd = fopen("0:/hello.txt", "r");
  if (fd) {
    struct file_stat s; 
    int res = fstat(fd, &s);
    if (res) {
      panic("ERR: fstat");
    }
    void* buf = kzalloc(s.file_size+1);
    if (!buf) {
      panic("ERR: allocating buf");
    }
    res = fread(buf, s.file_size, 1, fd);
    if (res != 1) {
      panic("ERR: fread");
    }
    print(buf);
    res = fclose(fd);
    if (res) {
      panic("ERR: fclose");
    }
    print("\nfile closed\n");
  }
}