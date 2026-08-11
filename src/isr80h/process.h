#ifndef ISR80H_PROCESS_H
#define ISR80H_PROCESS_H

#include "idt/idt.h"

void* isr80h_command6_process_load_start(struct interupt_frame* frame);
void* isr80h_command6_get_program_arguments(struct interupt_frame* frame);
void* isr80h_command7_invoke_system_command(struct interupt_frame* frame);

#endif