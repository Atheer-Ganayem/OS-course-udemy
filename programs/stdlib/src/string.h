#ifndef PEACHOS_STRING_H
#define PEACHOS_STRING_H

#include <stddef.h>
#include <stdbool.h>

size_t strlen(const char* str);
int strnlen(const char* ptr, int max);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, int count);
bool isdigit(char c);
int tonumericdigit(char c);
int strncmp(const char* str1, const char *str2, int n);
int istrncmp(const char* str1, const char *str2, int n);
int strnlen_terminator(const char* str, int max, char terminator);
char toLower(char c);
char* strtok(char* restrict str, const char* restrict delimiters);

#endif