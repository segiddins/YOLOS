#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include "string.h"
#include "termio.h"

/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main() {
    terminal_initialize();
    logf("YOLOS Logging initialized:\n");
    printf("Hello, kernel World!\n\tMy name is YOLOS\n%s\n", "I have come to rule the world!");
    printf("---\nKernel Loaded\n---\n");
}
