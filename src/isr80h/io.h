#ifndef ISR80H_IO_H
#define ISR80H_IO_H

#include "idt/idt.h"

void* isr80h_command1_print(struct interupt_frame* frame);
void* isr80h_command2_getkey(struct interupt_frame* frame);
void* isr80h_command3_putchar(struct interupt_frame* frame);

#endif
