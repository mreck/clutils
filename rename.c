#include <stdio.h>

#include "cli.h"
#include "cstr.h"

int main(int argc, char **argv) 
{
    char *program = shift_args(&argv, &argc);
    char *last_slash = cstr_find_last(program, '/');
    if (last_slash) program = last_slash + 1;
    printf("%s\n", program);

    char *arg;
    do {
        arg = shift_args(&argv, &argc);
        if (arg) printf("arg: %s\n", arg);
    } while(arg);

    return 0;
}
