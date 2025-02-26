#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cli.h"
#include "cstr.h"
#include "macros.h"

#define OPT_HELP        0
#define OPT_VERSION     1
#define OPT_VERBOSE     2
#define OPT_DRY_RUN     3
#define OPT_TIMESTAMP   4
#define OPT__LEN        5

CLI_Option opts[OPT__LEN];

void init_opts(void)
{
    opts[OPT_HELP] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'h',
        .long_cmd = "help",
        .env_cmd = NULL,
        .desc = "print the help message",
    };
    opts[OPT_VERSION] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = '\0',
        .long_cmd = "version",
        .env_cmd = NULL,
        .desc = "print the program version",
    };
    opts[OPT_VERBOSE] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'v',
        .long_cmd = "verbose",
        .env_cmd = NULL,
        .desc = "enable verbose logging",
    };
    opts[OPT_DRY_RUN] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'd',
        .long_cmd = "dry-run",
        .env_cmd = NULL,
        .desc = "don't backup any files, but print the results",
    };
    opts[OPT_TIMESTAMP] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 't',
        .long_cmd = "timestamp",
        .env_cmd = NULL,
        .desc = "use unix timestamps in the backup filename",
    };
}

int cmd_backup(char *filename)
{
    int result = 0;
    FILE *src = NULL;
    FILE *dst = NULL;
    char bak_fn[2048];
    int ret = snprintf(bak_fn, ARRAY_LENGTH(bak_fn), "%s.bak", filename);
    if (ret >= (int)ARRAY_LENGTH(bak_fn)) DEFER_WITH(CLU_ERR_BUFFER_TOO_SMALL);
    char *bak_fn_end = (char*)(bak_fn) + ret;
    int bak_fn_cap = (int)ARRAY_LENGTH(bak_fn) - ret;

    if (opts[OPT_TIMESTAMP].as.boolean) {
        unsigned long ts = time(NULL);
        ret = snprintf(bak_fn_end, bak_fn_cap, "_%lu", ts);
        if (ret >= bak_fn_cap) DEFER_WITH(CLU_ERR_BUFFER_TOO_SMALL);
    } else {
        if (cli_file_exists(bak_fn)) {
            int i = 0;
            do {
                ++i;
                ret = snprintf(bak_fn_end, bak_fn_cap, "%d", i);
                if (ret >= bak_fn_cap) DEFER_WITH(CLU_ERR_BUFFER_TOO_SMALL);
            } while (cli_file_exists(bak_fn));
        }
    }

    if (opts[OPT_DRY_RUN].as.boolean) {
        printf("[dry-run] \"%s\" -> \"%s\"\n", filename, bak_fn);
    } else {
        src = fopen(filename, "r+");
        if (!src) DEFER_WITH(errno);
        dst = fopen(bak_fn, "w+");
        if (!dst) DEFER_WITH(errno);
        char data[4096];
        size_t num_r, num_w;
        do {
            num_r = fread(data, 1, ARRAY_LENGTH(data), src);
            num_w = fwrite(data, 1, num_r, dst);
            if (num_w != num_r) DEFER_WITH(CLU_ERR_FILE_OP_FAILED);
        } while(num_r > 0);
        if (ferror(src) != 0 || ferror(dst) != 0) return CLU_ERR_FILE_OP_FAILED;
        if (opts[OPT_VERBOSE].as.boolean) {
            printf("\"%s\" -> \"%s\"\n", filename, bak_fn);
        }
    }

DEFER:
    if (src) fclose(src);
    if (dst) fclose(dst);
    return result;
}

int main(int raw_arg_cnt, char **raw_args)
{
    init_opts();

    char *program;
    char *args[256];
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

    if (opts[OPT_VERSION].as.boolean) {
        printf("%s version: 0.1.0\n", program); // @TODO: figure out a better option, like branch tags or something
    } else if (opts[OPT_HELP].as.boolean) {
        printf("NAME\n%s%s - a utility for backing up files\n\n", CLI_USAGE_SPACE, program);
        cli_print_options(opts, ARRAY_LENGTH(opts));
    } else {
        for (int i = 0; i < args_len; i++) {
            rval = cmd_backup(args[i]);
            if (rval > 0) {
                fprintf(stderr, "ERROR: %s\n", strerror(rval));
                return 1;
            }
            if (rval < 0) {
                fprintf(stderr, "ERROR: %s\n", cli_error_to_cstr(rval));
                return 1;
            }
        }
    }

    return 0;
}
