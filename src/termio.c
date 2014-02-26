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
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = make_vga_entry(' ', terminal_color);
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

void terminal_putchar(char c)
{
    if (!c) return;
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else if (c == '\t') {
        terminal_column += 4;
        if (terminal_column >= VGA_WIDTH)
        {
            terminal_column -= VGA_WIDTH;
            terminal_row++;
        }
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if ( ++terminal_column == VGA_WIDTH )
        {
            terminal_column = 0;
            if ( ++terminal_row == VGA_HEIGHT )
            {
                terminal_row = 0;
            }
        }
    }
}

void terminal_writestring(const char* data)
{
    size_t datalen = strlen(data);
    for ( size_t i = 0; i < datalen; i++ )
        terminal_putchar(data[i]);
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

int printf(char *format, ...)
{
    size_t len = strlen(format);
    char buffer[33];
    va_list list;
    va_start(list, format);
    for (unsigned int i = 0; i < len; i++)
    {
        char c = format[i];
        if (c != '%') {
            terminal_putchar(c);
        } else {
            c = format[++i];
            switch (c) {
                case 'd':
                case 'i':
                {
                    int num = va_arg(list, int);
                    terminal_writestring(itoa(num, buffer));
                    break;
                }

                case 'c':
                {
                    c = va_arg(list, int);
                    terminal_putchar(c);
                    break;
                }

                case 's':
                {
                    char *string = va_arg(list, char *);
                    terminal_writestring(string);
                    break;
                }

                case 'f':
                {
                    double num = va_arg(list, double);
                    terminal_writestring(itoa((int)num, buffer));
                    break;
                }

                default:
                {
                    printf("('%c' format specifier undefined in %s)", c, __PRETTY_FUNCTION__);
                    break;
                }
            }
        }
    }
    va_end(list);
    return 0;
}

void log_string(char *string)
{
    size_t len = strlen(string);
    for (unsigned int i = 0; i < len; ++i)
    {
        /* code */
        outb(COM, string[i]);
    }
}

void logf(char *format, ...)
{
    size_t len = strlen(format);
    char buffer[33];
    va_list list;
    va_start(list, format);
    for (unsigned int i = 0; i < len; i++)
    {
        char c = format[i];
        if (c != '%') {
            outb(COM, c);
        } else {
            c = format[++i];
            switch (c) {
                case 'd':
                case 'i':
                {
                    int num = va_arg(list, int);
                    log_string(itoa(num, buffer));
                    break;
                }

                case 'c':
                {
                    c = va_arg(list, int);
                    outb(COM, c);
                    break;
                }

                case 's':
                {
                    char *string = va_arg(list, char *);
                    log_string(string);
                    break;
                }
            }
        }
    }
    va_end(list);
}
