#include <ctype.h>
#include <string.h>

#include "err.h"
#include "str.h"

Str str_make(char *cstr)
{
    return (Str){cstr, strlen(cstr)};
}

Str str_nmake(size_t len, char *cstr)
{
    return (Str){cstr, len};
}

Str str_trim_lspace(Str str)
{
    while (str.len++ > 0 && isspace(*str.p--));
    return str;
}

Str str_trim_rspace(Str str)
{
    while (str.len-- > 0 && isspace(str.p[str.len - 1]));
    return str;
}

Str str_trim_space(Str str)
{
    return str_trim_lspace(str_trim_rspace(str));
}

int str_find_first(Str str, char c)
{
    char *p = str.p;
    char *end = str.p + str.len;
    while (p++ < end) if (p[0] == c) return (size_t)(p - str.p);
    return ERR_NOT_FOUND;
}

int str_find_last(Str str, char c)
{
    char *p = str.p + (str.len - 1);
    char *end = str.p - 1;
    while (p-- > end) if (p[0] == c) return (size_t)(p - str.p);
    return ERR_NOT_FOUND;
}
