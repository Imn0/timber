#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tmb_lib.h>

const char* default_format = "[ {D} ] {F} {L} - {M}";

#define HANDLE_ERROR(lbl, ...)                                                 \
    do {                                                                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        goto lbl;                                                              \
    } while (0)

static char* load_entire_file(const char* file) {
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        HANDLE_ERROR(error_return_only,
                     "cannot open file:\n\t%s\n",
                     strerror(errno));
    }

    if (fseek(f, 0L, SEEK_END) != 0) {
        HANDLE_ERROR(error_close,
                     "error reading file size:\n\t%s\n",
                     strerror(errno));
    }
    long buffsize = ftell(f);
    if (buffsize == -1) {
        HANDLE_ERROR(error_close,
                     "error reading file size:\n\t%s\n",
                     strerror(errno));
    }
    char* file_conents = malloc(sizeof(char) * buffsize);
    if (fseek(f, 0L, SEEK_SET) != 0) {
        HANDLE_ERROR(error_free,
                     "error reading file size:\n\t%s\n",
                     strerror(errno));
    }

    fread(file_conents, sizeof(char), buffsize, f);
    fclose(f);

    return file_conents;

error_free:
    free(file_conents);
error_close:
    fclose(f);
error_return_only:
    return NULL;
}

typedef struct {
    const char* start;
    int len;
    bool is_fn;
} CfgToken;

void process_token(CfgToken tok) {
    if (tok.len == 0) {
        // empty token
        return;
    }

    if (tok.is_fn) {
        if (strncmp(tok.start, "{D", 2) == 0) {
            printf("GOT D\n");
        } else if (strncmp(tok.start, "{L", 2) == 0) {
            printf("GOT L\n");
        } else if (strncmp(tok.start, "{M", 2) == 0) {
            printf("GOT M\n");
        } else if (strncmp(tok.start, "{F", 2) == 0) {
            printf("GOT F\n");
        }
    } else {
        printf(">%.*s<\n", tok.len, tok.start);
    }
}

void parse_config(const char* config_string) {
    int count = 0;
    const char* p = config_string;

    CfgToken tok = { .start = p, .len = 0 };
    while (*p) {
        // look out for '{'
        if (*p == '{') {
            process_token(tok);
            tok = (CfgToken) { .start = p, .len = 0, .is_fn = true };
            while (*p && *p != '}') {
                tok.len++;
                p++;
                if (*p == '{') {
                    printf("cannot have { inside another {\n");
                    return;
                }
            }
            if (*p == 0) {
                printf("unmatched {\n");
                return;
            }
            tok.len++;
            p++;

            process_token(tok);

            tok = (CfgToken) { .start = p, .len = 0 };
        } else {
            //everything outside '{}' is just normal textf
            tok.len++;
            p++;
        }
    }
}
