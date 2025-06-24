#ifndef CLUTIS_SBUF_H_
#define CLUTIS_SBUF_H_

#include <stddef.h>

#include "str.h"

typedef struct {
    char *p;
    size_t len;
    size_t cap;
} SBuf;

#define SBUF_REMAINING(b) ((b)->cap - (b)->len)

int  sbuf_init(SBuf *b, size_t cap);
int  sbuf_free(SBuf *b);
void sbuf_clear(SBuf *b);

Str  sbuf_get_str(SBuf *b);

int  sbuf_append_str(SBuf *b, Str str);
int  sbuf_prepend_str(SBuf *b, Str str);
int  sbuf_insert_str(SBuf *b, Str str, size_t pos);

#endif // CLUTIS_SBUF_H_
