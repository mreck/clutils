#ifndef CLUTIS_STR_H_
#define CLUTIS_STR_H_

#include <stddef.h>

typedef struct {
    char *p;
    size_t len;
} Str;

Str str_make(char *cstr);
Str str_nmake(size_t len, char *cstr);

Str str_trim_lspace(Str str);
Str str_trim_rspace(Str str);
Str str_trim_space(Str str);

int str_find_first(Str str, char c);
int str_find_last(Str str, char c);

#endif // CLUTIS_STR_H_
