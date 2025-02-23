#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "cstr.h"

char *cli_shift_args(char ***argv, int *argc)
{
    char *result = NULL;
    if (*argc > 0) {
        result = **argv;
        ++(*argv);
        --(*argc);
    }
    return result;
}

void cli_parse(char ***argv, int *argc, CLI_Option *opts, int optc, char **program)
{
    char *prog = cli_shift_args(argv, argc);
    if (program) {
        char *last_slash = cstr_find_last(prog, '/');
        if (last_slash) prog = last_slash + 1;
        *program = prog;
    }

    while (*argc > 0) {
        char *input = cli_shift_args(argv, argc);

        if (*input == '-') {
            ++input;
            if (*input == '-') {
                ++input;
                for (int i = 0; i < optc; i++) {
                    if (strcmp(input, opts[i].long_cmd) == 0) {
                        switch (opts[i].kind) {
                            case CLI_OPT_BOOL:
                                opts[i].as.boolean = true;
                                break;
                            case CLI_OPT_CSTR:
                                // @TODO
                                break;
                            default:
                                break;
                        }
                    }
                }
            } else {
                for (int i = 0; i < optc; i++) {
                    if (*input == opts[i].short_cmd) {
                        switch (opts[i].kind) {
                            case CLI_OPT_BOOL:
                                opts[i].as.boolean = true;
                                break;
                            case CLI_OPT_CSTR:
                                // @TODO
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        } else {
            // @TODO
        }
    }
}
