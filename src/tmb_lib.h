#ifndef TMB_LIB_H
#define TMB_LIB_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tmb.h>

typedef const char* cstr;

#ifdef _WIN32
    #define TMB_NEW_LINE "\n\r"
#else
    #define TMB_NEW_LINE "\n"
#endif

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

#define ASSERT assert

#define UNUSED (void)

#define TMB_DA_INIT_CAP 16

#define da_reserve(da, expected_cap)                                           \
    do {                                                                       \
        if ((expected_cap) > (da)->capacity) {                                 \
            if ((da)->capacity == 0) { (da)->capacity = TMB_DA_INIT_CAP; }     \
            while ((expected_cap) > (da)->capacity) { (da)->capacity *= 2; }   \
            void* new_items = realloc((da)->items,                             \
                                      ((unsigned long)(da)->capacity *         \
                                       sizeof(*(da)->items)));                 \
            if (new_items == NULL) {                                           \
                fprintf(stderr, "realloc faield");                             \
            } else {                                                           \
                (da)->items = new_items;                                       \
            }                                                                  \
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
        (da)->size += (new_items_size);                                        \
    } while (0)

#define da_last(da) (da)->items[ASSERT((da)->size > 0), (da)->size - 1]

#define da_for_each(T, it, da)                                                 \
    for (T * (it) = (da)->items; (it) < (da)->items + (da)->size; (it)++)

#define da_remove(da, idx)                                                     \
    do {                                                                       \
        size_t _m__i = (idx);                                                  \
        ASSERT(_m__i < (da)->size);                                            \
        (da)->items[j] = (da)->items[(da)->items[--(da)->size]]                \
    } while (0)

#define da_free(da)                                                            \
    do {                                                                       \
        free((da)->items);                                                     \
        (da)->capacity = 0;                                                    \
        (da)->size = 0;                                                        \
    } while (0);

typedef struct {
    const size_t size;
    const char items[];
} String;

typedef struct {
    char* items;
    size_t size;
    size_t capacity;
} StringBuilder;

#define sb_to_cstr(sb) da_append(sb, 0)

#define sb_appendn(sb, buff, n) da_appendn(sb, buff, n)

#define sb_append_cstr(sb, str)                                                \
    do {                                                                       \
        const cstr _m__s = (cstr)(str);                                        \
        int _m__n = (int)strlen(_m__s);                                        \
        da_appendn(sb, _m__s, _m__n);                                          \
    } while (0)

void sb_appendf(StringBuilder* sb, const cstr fmt, ...) TMB_FMT_CHECK(2, 3);
void sb_appendv(StringBuilder* sb, const cstr fmt, va_list args);
void do_nothing(void* _data);
String* make_string(const cstr str, size_t size);

/**
 * @brief Returns heap allocated zero terminated string with contents of the file
 * 
 * @param file
 * @return char*
 */
char* load_entire_file(const cstr file);

typedef void fmt_fn_t(StringBuilder* sb, const LogCtx* ctx, void* data);
typedef void sink_log_fn_t(const cstr msg, void* data);
typedef void free_fn_t(void* data);

#endif //TMB_LIB_H
