#include "memory.h"
#include "termio.h"
#include "string.h"

#define BUFFER_SIZE 10000

static char malloc_buffer[BUFFER_SIZE];
static size_t malloc_index = 0;

void log_string(const char *string);

void *malloc(size_t size)
{
  if (malloc_index + size >= BUFFER_SIZE)
  {
    log_string("attempted to malloc too much :(\n");
    return NULL;
  }
  void *start = (void *)(malloc_buffer+malloc_index);
  malloc_index += size;
  return start;
}

void free(void *ptr __attribute__((unused)))
{
}

void *calloc(size_t num_objs, size_t size)
{
  size_t real_size = num_objs * size;
  char *start = (char*)malloc(real_size);
  for (size_t i = 0; i < real_size; i++) {
    start[i] = 0;
  }
  return start;
}

void *realloc(void *ptr, size_t size)
{
  if (!size) {
    free(ptr);
    return NULL;
  }
  if (!ptr) {
    return malloc(size);
  }
  void *start = malloc(size);
  memcpy(ptr, start, size);
  free(ptr);
  return start;
}

