#include "termio.h"
#include "string.h"
#include <stdarg.h>

typedef enum
{
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
} vga_color;

uint8_t make_vga_color(vga_color fg, vga_color bg)
{
    return fg | bg << 4;
}

uint16_t make_vga_entry(char c, uint8_t color)
{
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

static const size_t VGA_HEIGHT = 24;
static const size_t VGA_WIDTH = 80;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*) 0xB8000; // Location of VGA text mode buffer
static uint16_t COM = 0x3F8;

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
    /* TODO: Is there any reason to force the use of eax and edx? */
}

void init_serial(uint16_t port) {
   outb(port + 1, 0x00);    // Disable all interrupts
   outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(port + 1, 0x00);    //                  (hi byte)
   outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

void terminal_initialize()
{
    init_serial(COM);
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = make_vga_color(COLOR_BLACK, COLOR_WHITE);
    for ( size_t y = 0; y < VGA_HEIGHT; y++ )
    {
        for ( size_t x = 0; x < VGA_WIDTH; x++ )
        {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = make_vga_entry(c, color);
}

void terminal_new_row()
{
    if (++terminal_row >= VGA_HEIGHT)
    {
        int bytes_to_copy = (VGA_HEIGHT - 1) * VGA_WIDTH;
        for (int i = 0; i < bytes_to_copy; ++i)
        {
            terminal_buffer[i] = terminal_buffer[i+VGA_WIDTH];
        }
        terminal_row = VGA_HEIGHT - 1;
        for (unsigned int i = 0; i < VGA_WIDTH; ++i)
        {
            terminal_putentryat(' ', terminal_color, i, terminal_row);
        }
    }
}

void terminal_putchar(char c)
{
    if (!c) return;
    if (c == '\n') {
        terminal_column = 0;
        terminal_new_row();
    } else if (c == '\t') {
        for (int i = 0; i < 4; ++i)
        {
            terminal_putchar(' ');
        }
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if ( ++terminal_column == VGA_WIDTH )
        {
            terminal_column = 0;
            terminal_new_row();
        }
    }
}

void terminal_writestring(const char* data)
{
    size_t datalen = strlen(data);
    for ( size_t i = 0; i < datalen; i++ )
        terminal_putchar(data[i]);
}

int printf(const char *format, ...)
{
    char *output = NULL;
    va_list list;
    va_start(list, format);
    int retval = vstrf(&output, format, list);
    va_end(list);
    terminal_writestring(output);
    return retval;
}

void log_string(char *string)
{
    size_t len = strlen(string);
    for (unsigned int i = 0; i < len; ++i)
    {
        outb(COM, string[i]);
    }
}

void logf(const char *format, ...)
{
    char *output = NULL;
    va_list list;
    va_start(list, format);
    vstrf(&output, format, list);
    va_end(list);
    log_string(output);
}
