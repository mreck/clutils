#ifndef CLUTIS_CSTR_H_
#define CLUTIS_CSTR_H_

#include <stdbool.h>

char *cstr_find_next_char(char *cstr, char c);
char *cstr_find_next_str(char *cstr, int cstr_len, char *pat, int pat_len);
char *cstr_find_last_char(char *cstr, char c);
bool  cstr_starts_with(char *cstr, char *needle);
int   cstr_replace_all(char *cstr, int cstr_len, char *pat, int pat_len, char *sub, int sub_len, char *buff, int buff_cap);

#endif // CLUTIS_CSTR_H_
