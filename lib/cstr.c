#include <stdbool.h>
#include <string.h>

#include "cstr.h"
#include "macros.h"

char *cstr_find_next_char(char *cstr, char c)
{
    char *next = NULL;
    while (*cstr != '\0') {
        if (*cstr == c) {
            next = cstr;
            break;
        }
        ++cstr;
    }
    return next;
}

char *cstr_find_next_str(char *cstr, int cstr_len, char *pat, int pat_len)
{
    int iter_len = cstr_len - pat_len + 1;
    for (int i = 0; i < iter_len; i++) {
        bool match = true;
        for (int j = 0; j < pat_len; j++) {
            if (!(cstr[i+j] == pat[j])) {
                match = false;
                break;
            }
        }
        if (match) return cstr + i;
    }
    return NULL;
}

char *cstr_find_last_char(char *cstr, char c)
{
    char *last = NULL;
    while (*cstr != '\0') {
        if (*cstr == c) last = cstr;
        ++cstr;
    }
    return last;
}

bool cstr_starts_with(char *cstr, char *needle)
{
    size_t len = strlen(needle);
    return strncmp(cstr, needle, len) == 0;
}

int cstr_replace_all(char *cstr, int cstr_len, char *pat, int pat_len, char *sub, int sub_len, char *buff, int buff_cap)
{
    // @TODO: implement interation
    int cnt = 0;
    char *pat_found = cstr_find_next_str(cstr, cstr_len, pat, pat_len);
    if (pat_found) {
        int required = strlen(cstr) - pat_len + sub_len;
        if (required < buff_cap) {
            int buff_offset = 0;
            memcpy(buff + buff_offset, cstr, pat_found - cstr);
            buff_offset += (pat_found - cstr);
            memcpy(buff + buff_offset, sub, sub_len);
            buff_offset += sub_len;
            strcpy(buff + buff_offset, pat_found + pat_len);
            ++cnt;
        } else {
            return CLU_ERR_BUFFER_TOO_SMALL;
        }
    }
    return cnt;
}
