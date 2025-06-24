#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"

char *err_get_msg(int err)
{
    switch (err) {
        case ERR_CAPACITY_TOO_SMALL: return "capacity too small";
        case ERR_OUT_OF_BOUNDS:      return "out of bounds";
        case ERR_NOT_FOUND:          return "not found";
    }
    return NULL;
}

void die(int err)
{
    char *msg = NULL;
    if (err > 0) msg = strerror(err);
    else if (err < 0) msg = err_get_msg(err);
    if (!msg) msg = "unknown";
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(1);
}

void die_if(int err)
{
    if (err != 0) die(err);
}
