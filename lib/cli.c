#include <stdlib.h>

#include "cli.h"

char *shift_args(char ***argv, int *argc)
{
    char *result = NULL;
    if (*argc > 0) {
        result = **argv;
        ++(*argv);
        --(*argc);
    }
    return result;
}
