#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tmb.h>

/*
[formats]
format_name="[ %date ] %message"
other_format="[ %date ] %message"
*/

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

TMB_API bool tmb_init(const char* config_file) {

    if (config_file == NULL) {
        // load default config here
        return false;
    }

    char* file_contents = load_entire_file(config_file);
    if (file_contents == NULL) { return false; }

    printf("%s", file_contents);

    // parse file
    // return logging handle

    return true;
}

TMB_API void tmb_log(void* logger, int log_level, const char* message, ...) {}
