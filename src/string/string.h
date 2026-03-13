#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include "stddef.h"

size_t strlen(const char* str);
int strnlen(const char* ptr, int max);
char* strcpy(char* dest, char* src);
bool isdigit(char c);
int tonumericdigit(char c);

#endif