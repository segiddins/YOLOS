// string.h
#ifndef _STRING_H
#define _STRING_H
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

size_t strlen(const char *str);

int memcmp(const void *str1, const void *str2, size_t n);

int strcmp(const char *str1, const char *str2);

int memcpy(void *in, void *out, size_t length);

void *memset(void *ptr, int value, size_t len);

int strf(char **out, const char *format, ...);
int vstrf(char **out, const char *format, va_list fargs);

#endif
