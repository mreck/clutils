#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cli.h"
#include "cstr.h"
#include "err.h"
#include "macros.h"
#include "sbuf.h"
#include "str.h"

#define OPT_HELP        0
#define OPT_VERSION     1
#define OPT_VERBOSE     2
#define OPT_INTERACTIVE 3
#define OPT_DRY_RUN     4
#define OPT_SUBSTITUTE  5
#define OPT_FORCE       6
#define OPT_BULK_EDIT   7
#define OPT_PREFIX      8
#define OPT__LEN        9

CLI_Option opts[OPT__LEN];

void init_opts(void)
{
    //                                    kind           short_cmd  long_cmd        env_cmd                 desc
    opts[OPT_VERSION]     = (CLI_Option){ CLI_OPT_BOOL,  '\0',      "version",      NULL,                   "print the program version",                      { NULL } };
    opts[OPT_VERBOSE]     = (CLI_Option){ CLI_OPT_BOOL,  'v',       "verbose",      NULL,                   "enable verbose logging",                         { NULL } };
    opts[OPT_HELP]        = (CLI_Option){ CLI_OPT_BOOL,  'h',       "help",         NULL,                   "print the help message",                         { NULL } };
    opts[OPT_INTERACTIVE] = (CLI_Option){ CLI_OPT_BOOL,  'i',       "interactive",  NULL,                   "rename files interactively, one by one",         { NULL } };
    opts[OPT_DRY_RUN]     = (CLI_Option){ CLI_OPT_BOOL,  'd',       "dry-run",      NULL,                   "don't rename any files, but print the results",  { NULL } };
    opts[OPT_SUBSTITUTE]  = (CLI_Option){ CLI_OPT_CSTR,  's',       "sub",          NULL,                   "substitute part of the filename",                { NULL } }; // @TODO: allow multiple
    opts[OPT_FORCE]       = (CLI_Option){ CLI_OPT_BOOL,  'f',       "force",        "RENAME_ALWAYS_FORCE",  "don't ask before overriding files",              { NULL } };
    opts[OPT_BULK_EDIT]   = (CLI_Option){ CLI_OPT_BOOL,  'b',       "bulk",         NULL,                   "bulk edit filenames using $EDITOR",              { NULL } };
    opts[OPT_PREFIX]      = (CLI_Option){ CLI_OPT_CSTR,  '\0',      "prefix",       NULL,                   "prefix the filenames",                           { NULL } };
}

int cmd_rename(char *old_path, char *new_path)
{
    char tmp_buf[1024];
    int err = 0;
    if (opts[OPT_DRY_RUN].as.boolean) {
        printf("[dry-run] \"%s\" -> \"%s\"\n", old_path, new_path);
    } else {
        bool override = opts[OPT_FORCE].as.boolean || !cli_file_exists(new_path);
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
    } else if (opts[OPT_HELP].as.boolean) {
        printf("NAME\n%s%s - a utility for renaming files\n\n", CLI_USAGE_SPACE, program);
        cli_print_options(opts, ARRAY_LENGTH(opts));
    } else if (opts[OPT_INTERACTIVE].as.boolean) {
        char in_buf[1024];
        for (int i = 0; i < args_len; i++) {
            printf("rename: \"%s\"\nto: ", args[i]);
            char *res = fgets(in_buf, ARRAY_LENGTH(in_buf), stdin);
            if (res != (char*)in_buf) {
               fprintf(stderr, "ERROR: invalid input\n");
               return 1;
            }
            die_if(cmd_rename(args[i], in_buf));
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
            rval = cstr_replace_all(args[i], (int)strlen(args[i]),
                                    fst + 1, pat_len,
                                    mid + 1, sub_len,
                                    sub_buf, ARRAY_LENGTH(sub_buf));
            if (rval > 0) {
                die_if(cmd_rename(args[i], sub_buf));
            } else if (rval < 0) {
                fprintf(stderr, "ERROR: %s\n", cli_error_to_cstr(rval));
                return 1;
            }
        }
    } else if (opts[OPT_PREFIX].as.cstr != NULL) {
        SBuf subst;
        die_if(sbuf_init(&subst, 1024));
        Str prefix = str_make(opts[OPT_PREFIX].as.cstr);
        for (int i = 0; i < args_len; i++) {
            Str arg = str_make(args[i]);
            int last = str_find_last(arg, '/');
            size_t pos = last < 0 ? 0 : last + 1;
            sbuf_clear(&subst);
            die_if(sbuf_append_str(&subst, arg));
            die_if(sbuf_insert_str(&subst, prefix, (size_t)pos));
            die_if(cmd_rename(arg.p, subst.p));
        }
    } else if (opts[OPT_BULK_EDIT].as.boolean) {
        if (getenv("EDITOR") == NULL) {
            fprintf(stderr, "ERROR: $EDITOR is not defined\n");
            return 1;
        }
        char temp_fn[256];
        snprintf(temp_fn, ARRAY_LENGTH(temp_fn), "%s/rename_%lu.txt", CLU_TEMP_DIR, (unsigned long)time(NULL)); // @TODO: handle buffer too small
        FILE *fp = fopen(temp_fn, "w+");
        if (!fp) {
            fprintf(stderr, "ERROR: creating temp file \"%s\" failed %s\n", temp_fn, strerror(rval));
            return 1;
        }
        for (int i = 0; i < args_len; i++) {
            if (fprintf(fp, "%s\n", args[i]) == 0) {
                fprintf(stderr, "ERROR: writing temp file \"%s\" failed %s\n", temp_fn, strerror(rval));
                fclose(fp);
                return 1;
            }
        }
        fclose(fp);
        rval = cli_open_editor(temp_fn);
        if (rval != 0) {
            fprintf(stderr, "ERROR: $EDITOR returned an error: %d\n", rval);
            return rval;
        }
        fp = fopen(temp_fn, "r+");
        if (!fp) {
            fprintf(stderr, "ERROR: creating temp file \"%s\" failed %s\n", temp_fn, strerror(rval));
            return 1;
        }
        char line_buff[2048];
        for (int i = 0; i < args_len; i++) {
            if (fgets(line_buff, ARRAY_LENGTH(line_buff), fp) == NULL) {
                fprintf(stderr, "ERROR: reading temp file \"%s\" failed %s\n", temp_fn, strerror(rval));
                fclose(fp);
                return 1;
            }
            size_t line_len = strlen(line_buff);
            if (line_buff[line_len-1] == '\n') {
                line_buff[line_len-1] = '\0';
            }
            if (strcmp(args[i], line_buff) != 0) {
                rval = cmd_rename(args[i], line_buff);
                if (rval != 0) {
                    fprintf(stderr, "ERROR: %s\n", strerror(rval));
                    fclose(fp);
                    return 1;
                }
            }
        }
        fclose(fp);
    }

    return 0;
}
