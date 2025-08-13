#ifndef TMB_LIB_H
#define TMB_LIB_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define TMB_NEW_LINE "\n\r"
#else
    #define TMB_NEW_LINE "\n"
#endif

#define ANSI_RESET     "0"
#define ANSI_BOLD      "1"
#define ANSI_DIM       "2"
#define ANSI_ITALIC    "3"
#define ANSI_UNDERLINE "4"
#define ANSI_BLINK     "5"
#define ANSI_REVERSE   "7"

#define ANSI_BLACK   "30"
#define ANSI_RED     "31"
#define ANSI_GREEN   "32"
#define ANSI_YELLOW  "33"
#define ANSI_BLUE    "34"
#define ANSI_MAGENTA "35"
#define ANSI_CYAN    "36"
#define ANSI_WHITE   "37"

#define ANSI_BG_BLACK   "40"
#define ANSI_BG_RED     "41"
#define ANSI_BG_GREEN   "42"
#define ANSI_BG_YELLOW  "43"
#define ANSI_BG_BLUE    "44"
#define ANSI_BG_MAGENTA "45"
#define ANSI_BG_CYAN    "46"
#define ANSI_BG_WHITE   "47"

#define ANSI_256(ID)    "38;5;" ID
#define ANSI_BG_256(ID) "48;5;" ID

#define MAKE_ANSI(...)                                                         \
    MAKE_ANSI_SELECT(__VA_ARGS__,                                              \
                     MAKE_ANSI6,                                               \
                     MAKE_ANSI5,                                               \
                     MAKE_ANSI4,                                               \
                     MAKE_ANSI3,                                               \
                     MAKE_ANSI2,                                               \
                     MAKE_ANSI1)(__VA_ARGS__)
#define MAKE_ANSI_SELECT(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define MAKE_ANSI1(a)                                       "\x1b[" a "m"
#define MAKE_ANSI2(a, b)                                    "\x1b[" a ";" b "m"
#define MAKE_ANSI3(a, b, c)                                 "\x1b[" a ";" b ";" c "m"
#define MAKE_ANSI4(a, b, c, d)                              "\x1b[" a ";" b ";" c ";" d "m"
#define MAKE_ANSI5(a, b, c, d, e)                           "\x1b[" a ";" b ";" c ";" d ";" e "m"

#if !defined(_MSC_VER) || defined(__clang__)
    // https://clang.llvm.org/docs/AttributeReference.html#format
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)                                  \
        __attribute__((format(printf, STR_IDX, ARG_BEGIN)))

    //https://clang.llvm.org/docs/AttributeReference.html#constructor-destructor
    #define TMB_INIT   __attribute__((constructor))
    #define TMB_DEINIT __attribute__((destructor))
#else // not supported on MSVC
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)
    #define TMB_INIT
    #define TMB_DEINIT
#endif

#define _da_header_(T)                                                         \
    size_t size;                                                               \
    size_t capacity;                                                           \
    T items

typedef struct tmb_string_builder {
    _da_header_(char*);
} tmb_string_builder_t;

typedef struct tmb_string_view {
    size_t size;
    char* items;
} tmb_string_view_t;

typedef void free_fn_t(void*);

#define UNREACHABLE()                                                          \
    do {                                                                       \
        UNUSED fprintf(stderr,                                                 \
                       "reached unreachable code at %s:%d",                    \
                       __FILE__,                                               \
                       __LINE__);                                              \
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

#define _da_header_(T)                                                         \
    size_t size;                                                               \
    size_t capacity;                                                           \
    T items

#define da_reserve_name(da, expected_cap, capacity_name, items_name)           \
    do {                                                                       \
        if ((expected_cap) > (da)->capacity_name) {                            \
            if ((da)->capacity_name == 0) {                                    \
                (da)->capacity_name = TMB_DA_INIT_CAP;                         \
            }                                                                  \
            while ((expected_cap) > (da)->capacity_name) {                     \
                (da)->capacity_name *= 2;                                      \
            }                                                                  \
            void* new_items = realloc((void*)(da)->items_name,                 \
                                      ((unsigned long)(da)->capacity_name *    \
                                       sizeof(*(da)->items_name)));            \
            if (new_items == NULL) {                                           \
                UNUSED fprintf(stderr, "realloc faield");                      \
            } else {                                                           \
                (da)->items_name = new_items;                                  \
            }                                                                  \
        }                                                                      \
    } while (0)

#define da_reserve(da, expected_cap)                                           \
    da_reserve_name(da, expected_cap, capacity, items)

#define da_append(da, item)                                                    \
    do {                                                                       \
        da_reserve(da, (da)->size + 1);                                        \
        (da)->items[(da)->size++] = (item);                                    \
    } while (0)

#define da_append_name(da, item, size_name, capacity_name, items_name)         \
    do {                                                                       \
        da_reserve_name(da, (da)->size_name + 1, capacity_name, items_name);   \
        (da)->items_name[(da)->size_name++] = (item);                          \
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
        if ((da)->capacity) {                                                  \
            free((da)->items);                                                 \
            (da)->capacity = 0;                                                \
            (da)->size     = 0;                                                \
            (da)->items    = NULL;                                             \
        }                                                                      \
    } while (0);

#define da_free_memb_fn(da, memb_data, memb_free_fn)                           \
    do {                                                                       \
        if ((da)->capacity) {                                                  \
            for (size_t i__m_ = 0; i__m_ < (da)->size; i__m_++) {              \
                (da)->items[i__m_].memb_free_fn((da)->items[i__m_].memb_data); \
            }                                                                  \
            free((da)->items);                                                 \
            (da)->capacity = 0;                                                \
            (da)->size     = 0;                                                \
        }                                                                      \
    } while (0);

#define string_free(str)                                                       \
    do {                                                                       \
        if ((str)->size) {                                                     \
            free((str)->items);                                                \
            (str)->size = 0;                                                   \
        }                                                                      \
    } while (0);

#define sb_to_cstr(sb) da_append(sb, 0)

#define sb_appendn(sb, buff, n) da_appendn(sb, buff, n)
#define sb_append(sb, chr)      da_append(sb, chr)
#define sb_free(sb)             da_free(sb)

#define sb_append_cstr(sb, str)                                                \
    do {                                                                       \
        const char* _m__s = (char*)(str);                                      \
        int _m__n         = (int)strlen(_m__s);                                \
        da_appendn(sb, _m__s, _m__n);                                          \
    } while (0)

void sb_appendf(tmb_string_builder_t* sb, const char* fmt, ...)
        TMB_FMT_CHECK(2, 3);
void sb_appendv(tmb_string_builder_t* sb, const char* fmt, va_list args);
void do_nothing(void* _data);

/**
 * @brief Returns heap allocated zero terminated string with contents of the file
 *
 * @param file
 * @return char*
 */
char* load_entire_file(const char* file);

#endif //TMB_LIB_H
