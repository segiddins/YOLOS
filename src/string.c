// string.c
#include "string.h"
#include "memory.h"

size_t strlen(const char *str)
{
    size_t ret = 0;
    while  (str[ret++]);
    return ret-1;
}

int memcmp(const void *str1, const void *str2, size_t n)
{
    int retval = 0;
    size_t index = 0;
    const char* string1 = str1;
    const char* string2 = str2;
    while (index < n && !retval) {
        char char1, char2;
        char1 = string1[index];
        char2 = string2[index];
        retval = char1 - char2;
        index++;
    }
    return retval;
}

int strcmp(const char *str1, const char *str2)
{
    size_t len1 = strlen(str1), len2 = strlen(str2);
    if (len1 != len2)
    {
        return len1 - len2;
    }
    return memcmp(str1, str2, len1);
}

int memcpy(void *in, void *out, size_t length)
{
    char* inc = in;
    char* outc = out;
    for (unsigned int i = 0; i < length; ++i)
    {
        outc[i] = inc[i];
    }
    return 0;
}

int strcpy_raw(char *in, char *out)
{
    size_t len = strlen(in);
    return memcpy(in, out, len);
}

int strcpy(char *in, char *out)
{
    size_t len = strlen(in);
    int retval = memcpy(in, out, len);
    out[len+1] = '\0';
    return retval;
}

char* itoa(int i, char b[])
{
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

char* ptoa(uintptr_t ptr, char b[])
{
  char const digit[] = "0123456789ABCDEF";
  char * p = b;
  uintptr_t shifter = ptr;
  do {
    ++p;
    shifter = shifter / 16;
  } while (shifter);
  p += 2;
  *p = '\0';
  do {
    *--p = digit[ptr % 16];
    ptr = ptr / 16;
  } while (ptr);
  *--p = 'x';
  *--p = '0';
  return b;
}

int strf(char **out, const char *format, ...)
{
    va_list list;
    va_start(list, format);
    int retval = vstrf(out, format, list);
    va_end(list);
    return retval;
}

int vstrf(char **out, const char *format, va_list fargs)
{
    size_t fmt_len = strlen(format);
    size_t len = 2 * fmt_len;
    size_t idx = 0;
    char *string = (char *)calloc(sizeof(char), len);
    char buffer[40];
    for (unsigned int i = 0; i <= fmt_len; i++)
    {
        char c = format[i];
        if (c != '%') {
            string[idx++] = c;
        } else {
            c = format[++i];
            switch (c) {
                case 'd':
                case 'i':
                {
                    int num = va_arg(fargs, int);
                    strcpy_raw(itoa(num, buffer), string + idx);
                    idx += strlen(buffer);
                    break;
                }

                case 'c':
                {
                    c = va_arg(fargs, int);
                    string[idx++] = c;
                    break;
                }

                case 's':
                {
                    char *s = va_arg(fargs, char *);
                    strcpy_raw(s, string + idx);
                    idx += strlen(s);
                    break;
                }

                case 'f':
                {
                    double num = va_arg(fargs, double);
                    strcpy_raw(itoa((int)num, buffer), string + idx);
                    idx += strlen(buffer);
                    break;
                }

                case 'p':
                {
                  uintptr_t num = va_arg(fargs, uintptr_t);
                  strcpy_raw(ptoa(num, buffer), string + idx);
                  idx += strlen(buffer);
                  break;
                }

                default:
                {
                    char *default_string = NULL;
                    strf(&default_string, "('%c' format specifier undefined in %s)", c, __PRETTY_FUNCTION__);
                    strcpy_raw(default_string, string + idx);
                    idx += strlen(default_string);
                    break;
                }
            }
        }

        if (len < idx) {
            len *= 2;
            idx = 0;
            i = 0;
            free(string);
            string = malloc(len);
        } else if (len - idx < len/2) {
            len *= 2;
            string = realloc(string, len);
        }
    }
    *out = string;
    return 0;
}
