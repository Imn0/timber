#include <stdio.h>
#include <tmb_lib.h>

void sb_appendf(StringBuilder* sb, const cstr fmt, ...) {
    va_list args;
    va_start(args, fmt);
    sb_appendv(sb, fmt, args);
    va_end(args);
}

void sb_appendv(StringBuilder* sb, const cstr fmt, va_list args) {

    va_list args1;
    va_copy(args1, args);

    int n = vsnprintf(NULL, 0, fmt, args1);
    va_end(args1);

    da_reserve(sb, sb->size + n + 1);

    va_copy(args1, args);
    vsnprintf(sb->items + sb->size, (unsigned long)n + 1, fmt, args1);
    va_end(args1);

    sb->size += n;
}

void do_nothing(void* _data) {
    UNUSED _data;
}

String* make_string(cstr str, unsigned long size) {
    struct {
        unsigned long size;
        char items[];
    }* string = malloc(sizeof(String) + size * sizeof(char));
    memcpy(string, &(String) { .size = size }, sizeof(String));
    memcpy(string->items, str, size);
    return (String*)string;
}

char* load_entire_file(const cstr file) {
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
    char* file_conents = malloc(sizeof(char) * (buffsize + 1));
    if (fseek(f, 0L, SEEK_SET) != 0) {
        HANDLE_ERROR(error_free,
                     "error reading file size:\n\t%s\n",
                     strerror(errno));
    }

    fread(file_conents, buffsize, 1, f);
    file_conents[buffsize] = '\0';
    fclose(f);

    printf(">%s<\n\r", file_conents);
    return file_conents;

error_free:
    free(file_conents);
error_close:
    fclose(f);
error_return_only:
    return NULL;
}
