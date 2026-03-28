#include "gdt.h"
#include "kernel.h"

void encodeGdtEntry(uint8_t* target, struct gdt_structured src) {
  if (src.limit > 65536 && (src.limit & 0xFFF) != 0xFFF) {
    panic("encodeGdtEntry: Invalid argument\n");
  }

  target[6] = 0x40;
  if (src.limit > 65536) {
    src.limit = src.limit >> 12;
    target[6] = 0xc0;
  }

  // limit 0-15
  target[0] = src.limit & 0xFF;
  target[1] = (src.limit >> 8) & 0xFF;
  
  // base 0-15 and 16-23 
  target[2] = src.base & 0xFF;
  target[3] = (src.base >> 8) & 0xFF;
  target[4] = (src.base >> 16) & 0xFF;

  target[5] = src.type;

  // limit 16 - 19
  target[6] |= (src.limit >> 16) & 0xFF;

  // base 24-31
  target[7] = (src.base >> 24) & 0xFF;
}

void gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt, int total_entries) {
  for (int i = 0; i < total_entries; i++) {
    encodeGdtEntry((uint8_t*)&gdt[i], structured_gdt[i]);
  }
}