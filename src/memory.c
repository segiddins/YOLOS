#include "memory.h"
#include "termio.h"
#include "string.h"

#define BUFFER_SIZE 100000

static char malloc_buffer[BUFFER_SIZE];
static uintptr_t malloc_index = 0;

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

// kmalloc

static char kmalloc_buffer[BUFFER_SIZE];
static uintptr_t kmalloc_index = 0;

void *kmalloc_int(size_t size, int align, void *physical)
{
  uintptr_t placement_address = (uintptr_t)(kmalloc_buffer + kmalloc_index);
  if (align == 1 && (placement_address & 0xFFFFF000)) {
    // align it
    placement_address &= 0xFFFFF000;
    placement_address += 0x1000;
  }
  if (physical) *(uintptr_t *)physical = placement_address;
  placement_address += size;
  kmalloc_index = placement_address - (uintptr_t)kmalloc_buffer;
  return (void *)(placement_address - size);
}

void *kmalloc_a(size_t size)
{
  return kmalloc_int(size, 1, NULL);
}

void *kmalloc_p(size_t size, void *physical)
{
  return kmalloc_int(size, 0, physical);
}

void *kmalloc_ap(size_t size, void *physical)
{
  return kmalloc_int(size, 1, physical);
}

void *kmalloc(size_t size)
{
  return kmalloc_int(size, 0, NULL);
}

