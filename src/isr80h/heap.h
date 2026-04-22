#ifndef ISR80H_HEAP_H
#define ISR80H_HEAP_H

#include "idt/idt.h"

void* isr80h_command4_malloc(struct interupt_frame* frame);
void* isr80h_command5_free(struct interupt_frame* frame);

#endif