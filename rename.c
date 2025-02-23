#include <stdio.h>

#include "cli.h"
#include "cstr.h"
#include "macros.h"

#define OPT_HELP 0
#define OPT_VERSION 1
#define OPT_VERBOSE 2
#define OPT_DIR 3
#define OPT__LEN 4

int main(int argc, char **argv) 
{
    char *program;
    CLI_Option opts[OPT__LEN];
    
    opts[OPT_VERBOSE] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'v',
        .long_cmd = "verbose",
        .desc = "enable verbose logging",
    };
    opts[OPT_DIR] = (CLI_Option){
        .kind = CLI_OPT_CSTR,
        .short_cmd = 'd',
        .long_cmd = "dir",
        .desc = "set dir",
    };
    opts[OPT_VERSION] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = '\0',
        .long_cmd = "version",
        .desc = "print the program version",
    };
    opts[OPT_HELP] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'h',
        .long_cmd = NULL,
        .desc = "print the help message",
    };

    int bad = cli_parse(argv, argc, opts, ARRAY_LENGTH(opts), &program);
    printf("%s\n", program);
    printf("verbose = %s\n", opts[OPT_VERBOSE].as.boolean ? "true" : "false");
    printf("dir = %s\n", opts[OPT_DIR].as.cstr);
    printf("bad = %d\n", bad);

    if (opts[OPT_HELP].as.boolean) {
        printf("\n\n");
        cli_print_options(opts, ARRAY_LENGTH(opts));
    }

    return 0;
}
