#include <stdio.h>

#include "cli.h"
#include "cstr.h"
#include "macros.h"

int main(int argc, char **argv) 
{
    char *program;
    CLI_Option opts[] = {
        {
            .kind = CLI_OPT_BOOL,
            .short_cmd = 'v',
            .long_cmd = "verbose",
            .desc = "enable verbose logging",
        },
    };

    cli_parse(&argv, &argc, opts, ARRAY_LENGTH(opts), &program);
    printf("%s\n", program);
    printf("verbose = %s\n", opts[0].as.boolean ? "true" : "false");

    return 0;
}
