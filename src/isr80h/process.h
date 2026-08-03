#ifndef ISR80H_PROCESS_H
#define ISR80H_PROCESS_H

#include "idt/idt.h"

void* isr80h_command6_process_load_start(struct interupt_frame* frame);

#endif