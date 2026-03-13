#include "string.h"

size_t strlen(const char* str) {
  size_t len = 0;
  while (*(str++)) len++;
  return len;
}

int strnlen(const char* ptr, int max) {
  int i = 0;
  for (i = 0; i < max; i++)
  {
    if (ptr[i] == 0)
      break;
  }

  return i;
}

char* strcpy(char* dest, char* src) {
  char* res = dest;
  while(*src) {
    *dest = *src;
    dest++; src++;
  }

  *dest = 0x00;

  return res;
}

bool isdigit(char c) {
  return c >= 48 && c <= 57;
}

int tonumericdigit(char c) {
  return c - 48;
}