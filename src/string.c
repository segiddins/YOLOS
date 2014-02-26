// string.c
#include "string.h"

size_t strlen(const char *str)
{
    size_t ret = 0;
    while  (str[ret++]);
    return ret;
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
