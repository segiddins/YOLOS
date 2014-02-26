// termio.h
#ifndef _TERMIO_H
#define _TERMIO_H
#include <stddef.h>
#include <stdint.h>

void terminal_initialize();

void logf(const char *format, ...);
int printf(const char *format, ...);

#endif
