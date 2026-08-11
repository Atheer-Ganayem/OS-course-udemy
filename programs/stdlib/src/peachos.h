#ifndef PEACHOS_H
#define PEACHOS_H

#include <stddef.h>
#include <stdbool.h>


struct process_arguments {
  int argc;
  char** argv;
};

struct command_argument {
  char argument[512];
  struct command_argument* next;
};

struct command_argument* peachos_parse_command(const char* command, int max);

void print(const char* message);
int peachos_getkey();
void peachos_putchar(char c);
void* peachos_malloc(size_t size);
void peachos_free(void* ptr);
int peachos_getkey_block();
void peachos_terminal_readline(char* out, size_t max, bool output_while_typing);
void peachos_process_load_start(const char* filename);
void peachos_process_get_arguments(struct process_arguments* arguments);

#endif