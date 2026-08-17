#include "peachos.h"
#include "string.h"

struct command_argument* peachos_parse_command(const char* command, int max) {
  char scommand[1024];

  if (max > sizeof(scommand)) {
    return NULL;
  }

  strncpy(scommand, command, sizeof(scommand));
  
  char* token = strtok(scommand, " ");
  if (!token) {
    return NULL;
  }

  struct command_argument* root_command = peachos_malloc(sizeof(struct command_argument));
  if (!root_command) {
    return NULL;
  }

  strncpy(root_command->argument, token, sizeof(root_command->argument));
  root_command->next = NULL;

  struct command_argument* current = root_command;
  token = strtok(NULL, " ");
  while (token) {
    struct command_argument* new_command = peachos_malloc(sizeof(struct command_argument));
    if (!new_command) {
      break;
    }

    strncpy(new_command->argument, token, sizeof(new_command->argument));
    new_command->next = NULL;
    current->next = new_command;
    current = current->next;

    token = strtok(NULL, " ");
  }

out:
  return root_command;
}

int peachos_getkey_block() {
  int val;
  do {
    val = peachos_getkey();
  } while (!val);
  
  return val;
}

void peachos_terminal_readline(char* out, size_t max, bool output_while_typing) {
  int i = 0;
  
  // saving last byte for null terminator 
  for (i = 0; i < max-1; i++) {
    int c = peachos_getkey_block();

    if (c == 13) {
      break;
    }

    if (c == 0x08 && i == 0) {
      i--;
    } else if (c == 0x08 && i > 0) { // Backspace
      if (output_while_typing) peachos_putchar(c);
      // decreasing i twice because after before we loop again we are gonna i++
      out[--i] = 0x00;
      --i;
    } else if (c != 0x08) {
      out[i] = c;
      if (output_while_typing) peachos_putchar(c);
    }
  }

  out[i] = 0x00;
}

int peachos_system_run(const char* command) {
  char buf[1024];
  strncpy(buf, command, sizeof(buf));
  struct command_argument* root_cmd_arg = peachos_parse_command(command, sizeof(buf));
  if (!root_cmd_arg) {
    return -1;
  }

  return peachos_system(root_cmd_arg);
  
}