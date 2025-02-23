#ifndef CLUTIS_CLI_H_
#define CLUTIS_CLI_H_

#include <stdbool.h>

#define CLI_USAGE_SPACE "       "

#define CLI_OPT_BOOL 1
#define CLI_OPT_CSTR 2

typedef struct {
    int kind;
    char short_cmd;
    char *long_cmd;
    char *desc;
    union {
        bool boolean;
        char *cstr;
    } as;
} CLI_Option;

char *cli_shift_args(char ***argv, int *argc);
int cli_parse(char **argv, int argc, CLI_Option *opts, int optc, char **program);
void cli_print_options(CLI_Option *opts, int optc);

#endif // CLUTIS_CLI_H_
