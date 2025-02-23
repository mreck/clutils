#include <stdio.h>
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

int cli_parse(char **argv, int argc, CLI_Option *opts, int optc, char **program)
{
    char *prog = argv[0];
    if (program) {
        char *last_slash = cstr_find_last(prog, '/');
        if (last_slash) prog = last_slash + 1;
        *program = prog;
    }

    for (int argi = 1; argi < argc; argi++) {
        char *input = argv[argi];
        if (*input == '-') {
            ++input;
            if (*input == '-') {
                ++input;
                bool match = false;
                for (int i = 0; i < optc; i++) {
                    if (opts[i].long_cmd && cstr_starts_with(input, opts[i].long_cmd)) {
                        switch (opts[i].kind) {
                            case CLI_OPT_BOOL:
                                if (strcmp(input, opts[i].long_cmd) == 0) {
                                    opts[i].as.boolean = true;
                                    match = true;
                                }
                                break;
                            case CLI_OPT_CSTR:
                                input += strlen(opts[i].long_cmd);
                                if (*input == '=') {
                                    opts[i].as.cstr = input + 1;
                                    match = true;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    if (match) break;
                }
                if (!match) return argi;
            } else {
                while (input && *input) {
                    bool match = false;
                    for (int i = 0; i < optc; i++) {
                        if (opts[i].short_cmd && *input == opts[i].short_cmd) {
                            switch (opts[i].kind) {
                                case CLI_OPT_BOOL:
                                    opts[i].as.boolean = true;
                                    ++input;
                                    break;
                                case CLI_OPT_CSTR:
                                    opts[i].as.cstr = input + 1;
                                    input = NULL;
                                    break;
                                default:
                                    break;
                            }
                        }
                        if (match) break;
                    }
                    if (!match) return argi;
                }
            }
        } else {
            // @TODO: implement non-opt handling
        }
    }

    return 0;
}

void cli_print_options(CLI_Option *opts, int optc)
{
    printf("OPTIONS\n");
    for (int i = 0; i < optc; i++) {
        if (opts[i].short_cmd && opts[i].long_cmd)
            printf("%s-%c, --%s\n", CLI_USAGE_SPACE, opts[i].short_cmd, opts[i].long_cmd);
        else if (opts[i].short_cmd)
            printf("%s-%c\n", CLI_USAGE_SPACE, opts[i].short_cmd);
        else 
            printf("%s--%s\n", CLI_USAGE_SPACE, opts[i].long_cmd);
        printf("%s%s%s\n\n", CLI_USAGE_SPACE, CLI_USAGE_SPACE, opts[i].desc);
    }
}
