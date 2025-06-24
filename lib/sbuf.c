#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "sbuf.h"
#include "err.h"

int sbuf_init(SBuf *b, size_t cap)
{
    if (cap < 1) return ERR_CAPACITY_TOO_SMALL;
    char *p = (char*)malloc(cap);
    if (!p) return errno;
    p[0] = '\0';
    b->p = p;
    b->len = 0;
    b->cap = cap;
    return 0;
}

int sbuf_free(SBuf *b)
{
    if (b->p) free(b->p);
    b->p = NULL;
    b->len = 0;
    b->cap = 0;
    return 0;
}

void sbuf_clear(SBuf *b)
{
    b->len = 0;
}

Str sbuf_get_str(SBuf *b)
{
    return (Str){b->p, b->len};
}

int sbuf_append_str(SBuf *b, Str str)
{
    if (SBUF_REMAINING(b) < str.len + 1) return ERR_CAPACITY_TOO_SMALL;
    memcpy(b->p + b->len, str.p, str.len);
    b->len += str.len;
    b->p[b->len] = '\0';
    return 0;
}

int sbuf_prepend_str(SBuf *b, Str str)
{
    if (SBUF_REMAINING(b) < str.len + 1) return ERR_CAPACITY_TOO_SMALL;
    memmove(b->p + str.len, b->p, b->len + 1);
    memcpy(b->p, str.p, str.len);
    return 0;
}

int sbuf_insert_str(SBuf *b, Str str, size_t pos)
{
    if (SBUF_REMAINING(b) < str.len + 1) return ERR_CAPACITY_TOO_SMALL;
    if (pos > b->len - 1) return ERR_OUT_OF_BOUNDS;
    memmove(b->p + pos + str.len, b->p + pos, b->len - pos + 1);
    memcpy(b->p + pos, str.p, str.len);
    return 0;
}
