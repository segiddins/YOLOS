// memory.h

#ifndef _MEMORY_H
#define _MEMORY_H

#include <stddef.h>
#include <stdint.h>

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t num_objs, size_t size);
void *realloc(void *ptr, size_t size);

#endif
