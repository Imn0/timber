#include <tmb_lib.h>

#include <stdio.h>

void sb_appendf(tmb_string_builder_t* sb, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    sb_appendv(sb, fmt, args);
    va_end(args);
}

void sb_appendv(tmb_string_builder_t* sb, const char* fmt, va_list args) {

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

char* load_entire_file(const char* file) {
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        perror("cannot open file");
        goto error_return_only;
    }

    if (fseek(f, 0L, SEEK_END) != 0) {
        goto error_close;
        perror("error reading file size");
    }
    long buffsize = ftell(f);
    if (buffsize == -1) {
        goto error_close;
        perror("error reading file size");
    }
    char* file_conents = malloc(sizeof(char) * (buffsize + 1));
    if (fseek(f, 0L, SEEK_SET) != 0) {
        goto error_free;
        perror("error reading file size");
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

// inline tmb_time_stamp_t tmb_timestamp() {
//     size_t sec  = 0;
//     size_t nsec = 0;

// #if defined(TMB_UNIX)
//     struct timespec ts = { 0 };
//     clock_gettime(CLOCK_REALTIME, &ts);
//     sec  = ts.tv_sec;
//     nsec = ts.tv_nsec;
// #elif defined(TMB_WINDOWS)
//     const time_t ts;
//     sec = time(NULL);
// #else
//     sec = time(NULL);
// #endif

//     return (tmb_time_stamp_t) { .sec = sec, .nsec = nsec };
// }
