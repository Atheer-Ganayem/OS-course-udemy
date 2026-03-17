#include "string.h"

char toLower(char c) {
  if (c >= 65 && c <= 90) {
    return c + 32;
  }
  return c;
}


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

int istrncmp(const char* str1, const char *str2, int n) {
  unsigned char u1, u2;
  while (n-- > 0) {
    u1 = (unsigned char)*str1++;
    u2 = (unsigned char)*str2++;
    if (u1 != u2 && toLower(u1) != toLower(u2)) {
      return u1-u2;
    }
    if (u1 == 0x00) {
      return 0;
    }
  }

  return 0;
}

int strncmp(const char* str1, const char *str2, int n) {
  unsigned char u1, u2;
  while (n-- > 0) {
    u1 = (unsigned char)*str1++;
    u2 = (unsigned char)*str2++;
    if (u1 != u2) {
      return u1-u2;
    }
    if (u1 == 0x00) {
      return 0;
    }
  }

  return 0;
}

int strnlen_terminator(const char* str, int max, char terminator) {
  int i = 0;
  for (; i < max; i++) {
    if (!str[i] || str[i] == terminator) {
      break;
    }
  }

  return i;
}

bool isdigit(char c) {
  return c >= 48 && c <= 57;
}

int tonumericdigit(char c) {
  return c - 48;
}