#ifndef TMB_LIB_H
#define TMB_LIB_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tmb.h>

typedef char* cstr;

#define UNREACHABLE()                                                          \
    do {                                                                       \
        fprintf(stderr,                                                        \
                "reached unreachable code at %s:%d",                           \
                __FILE__,                                                      \
                __LINE__);                                                     \
        abort();                                                               \
    } while (0)

#define TODO(...)                                                              \
    do {                                                                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        exit(1);                                                               \
    } while (0)

#define UNUSED (void)

#define TMB_DA_INIT_CAP 1

#define da_reserve(da, expected_cap)                                           \
    do {                                                                       \
        if ((expected_cap) > (da)->capacity) {                                 \
            if ((da)->capacity == 0) { (da)->capacity = TMB_DA_INIT_CAP; }     \
            while ((expected_cap) > (da)->capacity) { (da)->capacity *= 2; }   \
            (da)->items = realloc((da)->items,                                 \
                                  ((unsigned long)(da)->capacity *             \
                                   sizeof(*(da)->items)));                     \
        }                                                                      \
    } while (0)

#define da_append(da, item)                                                    \
    do {                                                                       \
        da_reserve(da, (da)->size + 1);                                        \
        (da)->items[(da)->size++] = (item);                                    \
    } while (0)

#define da_appendn(da, new_items, new_items_size)                              \
    do {                                                                       \
        da_reserve((da), (da)->size + (new_items_size));                       \
        memcpy((da)->items + (da)->size,                                       \
               (new_items),                                                    \
               (unsigned long)(new_items_size) * sizeof(*(da)->items));        \
        (da)->size += new_items_size;                                          \
    } while (0)

#define da_free(da)                                                            \
    do {                                                                       \
        free((da)->items);                                                     \
        (da)->capacity = 0;                                                    \
        (da)->size = 0;                                                        \
    } while (0);

typedef struct {
    char* items;
    int size;
    int capacity;
} StringBuilder;

#define sb_to_cstr(sb) da_append(sb, 0)

#define sb_appendn(sb, buff, n) da_appendn(sb, buff, n)

#define sb_append_cstr(sb, cstr)                                               \
    do {                                                                       \
        const char* __s = (char*)cstr;                                         \
        int __n = (int)strlen(__s);                                            \
        da_appendn(sb, __s, __n);                                              \
    } while (0)

void sb_appendf(StringBuilder* sb, const char* fmt, ...) TMB_FMT_CHECK(2, 3);
void sb_appendv(StringBuilder* sb, const char* fmt, va_list args);
#endif //TMB_LIB_H
