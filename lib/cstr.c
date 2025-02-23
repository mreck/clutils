#include <string.h>

#include "cstr.h"

char *cstr_find_last(char *cstr, char c)
{
    char *last = NULL;
    while (*cstr != '\0') {
        if (*cstr == c) last = cstr;
        ++cstr;
    }
    return last;
}
