#ifndef CLUTIS_CLI_H_
#define CLUTIS_CLI_H_

#include <stdbool.h>

#ifndef CLI_USAGE_SPACE
#define CLI_USAGE_SPACE "       "
#endif

#define CLI_OPT_BOOL 1
#define CLI_OPT_CSTR 2

typedef struct {
    int kind;
    char short_cmd;
    char *long_cmd;
    char *env_cmd;
    char *desc;
    union {
        bool boolean;
        char *cstr;
    } as;
} CLI_Option;

char *cli_shift_args(char ***argv, int *argc);
int   cli_parse(char **raw_args, int raw_arg_cnt, CLI_Option *opts, int optc, char **args, int args_cap, int *args_len, char **program);
void  cli_print_options(CLI_Option *opts, int optc);
char *cli_error_to_cstr(int err);
bool  cli_prompt_confirm(char *question);
int   cli_open_editor(char *filename);
bool  cli_file_exists(char *filename);

#endif // CLUTIS_CLI_H_
