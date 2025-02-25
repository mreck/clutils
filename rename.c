#include <stdio.h>

#include "cli.h"
#include "cstr.h"
#include "macros.h"

#define OPT_HELP 0
#define OPT_VERSION 1
#define OPT_VERBOSE 2
#define OPT_INTERACTIVE 3
#define OPT_DRY_RUN 4
#define OPT__LEN 5

CLI_Option opts[OPT__LEN];

void init_opts(void)
{
    opts[OPT_VERSION] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = '\0',
        .long_cmd = "version",
        .desc = "print the program version",
    };
    opts[OPT_VERBOSE] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'v',
        .long_cmd = "verbose",
        .desc = "enable verbose logging",
    };
    opts[OPT_HELP] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'h',
        .long_cmd = "help",
        .desc = "print the help message",
    };
    opts[OPT_INTERACTIVE] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'i',
        .long_cmd = "interactive",
        .desc = "rename files interactively, one by one",
    };
    opts[OPT_DRY_RUN] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'd',
        .long_cmd = "dry-run",
        .desc = "don't rename any files, but print the results",
    };
}

int main(int raw_arg_cnt, char **raw_args) 
{
    init_opts();

    char *program;
    char *args[512];
    int args_len;
    int rval = cli_parse(raw_args, raw_arg_cnt, opts, ARRAY_LENGTH(opts), args, ARRAY_LENGTH(args), &args_len, &program);
    if (rval != 0) {
        if (rval > 0) {
            fprintf(stderr, "ERROR: invalid option: %s\n", raw_args[rval]);
        } else {
            fprintf(stderr, "ERROR: %s\n", cli_error_to_cstr(rval));
        }
        return 1;
    }

    char in_buf[1024];
    if (opts[OPT_INTERACTIVE].as.boolean) {
        for (int i = 0; i < args_len; i++) {
            printf("rename: %s\nto: ", args[i]);
            char *res = fgets(in_buf, ARRAY_LENGTH(in_buf), stdin);
            if (res != (char*)in_buf) {
               fprintf(stderr, "ERROR: invalid input\n");
               return 1;
            }
            if (opts[OPT_DRY_RUN].as.boolean) {
                printf("%s -> %s\n", args[i], in_buf);
            }
        }
    }

    return 0;
}
