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

int cli_parse(char **raw_args, int raw_arg_cnt, CLI_Option *opts, int optc, char **args, int args_cap, int *args_len, char **program)
{
    char *prog = raw_args[0];
    if (program) {
        char *last_slash = cstr_find_last(prog, '/');
        if (last_slash) prog = last_slash + 1;
        *program = prog;
    }

    *args_len = 0;
    for (int argi = 1; argi < raw_arg_cnt; argi++) {
        char *input = raw_args[argi];
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
                        }
                    }
                    if (match) break;
                }
                if (!match) return argi;
            } else {
                bool match = false;
                while (input && *input) {
                    match = false;
                    for (int i = 0; i < optc; i++) {
                        if (opts[i].short_cmd && *input == opts[i].short_cmd) {
                            switch (opts[i].kind) {
                                case CLI_OPT_BOOL:
                                    opts[i].as.boolean = true;
                                    ++input;
                                    match = true;
                                    break;
                                case CLI_OPT_CSTR:
                                    opts[i].as.cstr = input + 1;
                                    input = NULL;
                                    match = true;
                                    break;
                            }
                        }
                        if (match) break;
                    }
                }
                if (!match) return argi;
            }
        } else {
            if ((*args_len) < args_cap) {
                args[(*args_len)++] = input;
            } else {
                return CLI_ERR_TOO_MANY_ARGS;
            }
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

char *cli_error_to_cstr(int err)
{
    switch (err) {
        case CLI_ERR_TOO_MANY_ARGS: return "too many arguments";
    }
    return "unknown error";
}
