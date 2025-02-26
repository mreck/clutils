#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "cli.h"
#include "cstr.h"
#include "macros.h"

#define OPT_HELP        0
#define OPT_VERSION     1
#define OPT_VERBOSE     2
#define OPT_INTERACTIVE 3
#define OPT_DRY_RUN     4
#define OPT_SUBSTITUTE  5
#define OPT_FORCE       6
#define OPT__LEN        7

CLI_Option opts[OPT__LEN];

void init_opts(void)
{
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
    opts[OPT_HELP] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'h',
        .long_cmd = "help",
        .env_cmd = NULL,
        .desc = "print the help message",
    };
    opts[OPT_INTERACTIVE] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'i',
        .long_cmd = "interactive",
        .env_cmd = NULL,
        .desc = "rename files interactively, one by one",
    };
    opts[OPT_DRY_RUN] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'd',
        .long_cmd = "dry-run",
        .env_cmd = NULL,
        .desc = "don't rename any files, but print the results",
    };
    opts[OPT_SUBSTITUTE] = (CLI_Option){ // @TODO: allow multiple
        .kind = CLI_OPT_CSTR,
        .short_cmd = 's',
        .long_cmd = "sub",
        .env_cmd = NULL,
        .desc = "substitute part of the filename",
    };
    opts[OPT_FORCE] = (CLI_Option){
        .kind = CLI_OPT_BOOL,
        .short_cmd = 'f',
        .long_cmd = "force",
        .env_cmd = "RENAME_ALWAYS_FORCE",
        .desc = "don't ask before overriding files",
    };
}

int cmd_rename(char *old_path, char *new_path)
{
    char tmp_buf[1024];
    int err = 0;
    if (opts[OPT_DRY_RUN].as.boolean) {
        printf("[dry-run] \"%s\" -> \"%s\"\n", old_path, new_path);
    } else {
        bool override = opts[OPT_FORCE].as.boolean || access(new_path, F_OK) != 0;
        if (!override) {
            snprintf(tmp_buf, ARRAY_LENGTH(tmp_buf),  "override \"%s\"?", new_path);
            override = cli_prompt_confirm(tmp_buf);
        }
        if (override) {
            if (rename(old_path, new_path) != 0) {
                err = errno;
            }
            if (err == 0 && opts[OPT_VERBOSE].as.boolean) {
                printf("\"%s\" -> \"%s\"\n", old_path, new_path);
            }
        }
    }
    return err;
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

    if (opts[OPT_VERSION].as.boolean) {
        printf("%s version: 0.1.0\n", program); // @TODO: figure out a better option, like branch tags or something
    } else if (opts[OPT_INTERACTIVE].as.boolean) {
        char in_buf[1024];
        for (int i = 0; i < args_len; i++) {
            printf("rename: \"%s\"\nto: ", args[i]);
            char *res = fgets(in_buf, ARRAY_LENGTH(in_buf), stdin);
            if (res != (char*)in_buf) {
               fprintf(stderr, "ERROR: invalid input\n");
               return 1;
            }
            rval = cmd_rename(args[i], in_buf);
            if (rval != 0) {
                fprintf(stderr, "ERROR: %s\n", strerror(rval));
                return 1;
            }
        }
    } else if (opts[OPT_SUBSTITUTE].as.cstr != NULL) {
        char *p = opts[OPT_SUBSTITUTE].as.cstr;
        char sep = *p;
        char *fst = p;
        char *mid = cstr_find_next_char(fst+1, sep);
        if (!mid) {
            fprintf(stderr, "ERROR: invalid substitute: %s\n", opts[OPT_SUBSTITUTE].as.cstr);
            return 1;
        }
        char *end = cstr_find_next_char(mid+1, sep);
        if (!end || end[1] != '\0') {
            fprintf(stderr, "ERROR: invalid substitute: %s\n", opts[OPT_SUBSTITUTE].as.cstr);
            return 1;
        }
        int pat_len = mid - fst - 1;
        int sub_len = end - mid - 1;
        for (int i = 0; i < args_len; i++) {
            char sub_buf[1024];
            int cnt = cstr_replace_all(args[i], (int)strlen(args[i]),
                                       fst + 1, pat_len,
                                       mid + 1, sub_len,
                                       sub_buf, ARRAY_LENGTH(sub_buf));
            if (cnt > 0) {
                rval = cmd_rename(args[i], sub_buf);
                if (rval != 0) {
                    fprintf(stderr, "ERROR: %s\n", strerror(rval));
                    return 1;
                }
            }
        }
    }

    return 0;
}
