#ifndef TMB_LIB_H_
#define TMB_LIB_H_

#if defined(_WIN32)
    #define TMB_WINDOWS
    #if defined(__clang__) || (__GNUC__)
        #define TMB_WINDOWS_GNU
    #elif defined(_MSC_VER)
        #define TMB_WINDOWS_MSVC
    #endif
#elif defined(__unix__)
    #define TMB_UNIX
    #define _POSIX_C_SOURCE 199309L
#else
    #error "unknown os"
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
                     MAKE_ANSI1,                                               \
                     MAKE_ANSI0)(__VA_ARGS__)
#define MAKE_ANSI_SELECT(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define MAKE_ANSI0()                                        ""
#define MAKE_ANSI1(a)                                       "\x1b[" a "m"
#define MAKE_ANSI2(a, b)                                    "\x1b[" a ";" b "m"
#define MAKE_ANSI3(a, b, c)                                 "\x1b[" a ";" b ";" c "m"
#define MAKE_ANSI4(a, b, c, d)                              "\x1b[" a ";" b ";" c ";" d "m"
#define MAKE_ANSI5(a, b, c, d, e)                           "\x1b[" a ";" b ";" c ";" d ";" e "m"

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef size_t usize;

#ifdef TMB_WINDOWS
    #define TMB_NEW_LINE "\n\r"
#else
    #define TMB_NEW_LINE "\n"
#endif

#if defined(TMB_UNIX) || defined(TMB_WINDOWS_GNU)
    // https://clang.llvm.org/docs/AttributeReference.html#format
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)                                  \
        __attribute__((format(printf, STR_IDX, ARG_BEGIN)))

    // https://clang.llvm.org/docs/AttributeReference.html#constructor-destructor
    #define TMB_INIT   __attribute__((constructor))
    #define TMB_DEINIT __attribute__((destructor))
#else // not supported on MSVC
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)
    #define TMB_INIT
    #define TMB_DEINIT
#endif
#if defined(TMB_WINDOWS_MSVC)
__declspec(noreturn) inline void unreachable() {
    __assume(false);
}
#elif defined(TMB_UNIX) || defined(TMB_WINDOWS_GNU)
    #ifndef unreachable
        #define unreachable() __builtin_unreachable()
    #endif
#else
inline void unreachable() {
    abort();
}
#endif

#define _da_header_(T)                                                         \
    int length;                                                                \
    int capacity;                                                              \
    T items

typedef struct tmb_string_builder {
    _da_header_(char*);
} tmb_string_builder_t;

typedef struct tmb_string_view {
    int length;
    const char* items;
} tmb_string_view_t;

#define NSEC_IN_SEC  1000000000
#define NSEC_IN_MSEC 1000000

#define MSEC_TO_NSEC(msec) ((msec) * NSEC_IN_MSEC)

typedef struct tmb_time_stamp {
    i64 sec;
    i64 nsec;
} tmb_time_stamp_t;

f32 tmb_time_stamp_diff(tmb_time_stamp_t ts1, tmb_time_stamp_t ts2);
tmb_time_stamp_t tmb_time_stamp();
tmb_time_stamp_t tmb_time_stopwatch();

struct tmb_cmp_flt_opt {
    f32 eps;
};
bool tmb_cmp_flt_impl(f32 a, f32 b, struct tmb_cmp_flt_opt);
#define tmb_cmp_flt(a, b, ...)                                                 \
    tmb_cmp_flt_impl(                                                          \
            (a), (b), (struct tmb_cmp_flt_opt) { .eps = 1e-9, __VA_ARGS__ })

enum tmb_sb_just_opt {
    JUST_OFF    = 0,
    JUST_LEFT   = 1,
    JUST_RIGHT  = 2,
    JUST_CENTER = 3
};
enum tmb_sb_truncate_opt {
    TRUNCATE_OFF   = 0,
    TRUNCATE_LEFT  = 1,
    TRUNCATE_RIGHT = 2
};

typedef struct tmb_hm_opt {
    int initial_size;
} tmb_hm_opt_t;
enum tmb_hm_key_type__ { KEY_RAW = 1, KEY_STR = 2 };

#define TMB_DISTINGUISH_HM_TYPE(x)                                             \
    _Generic((x), char*: KEY_STR, const char*: KEY_STR, default: KEY_RAW)

#if defined(TMB_WINDOWS_MSVC)
    #define TMB_TYPEOF(x)            __typeof__(x)
    #define TMB_OFFSETOF(var, field) offsetof(__typeof__(*(var)), field)
#else
    #define TMB_TYPEOF(x)            typeof(x)
    #define TMB_OFFSETOF(var, field) offsetof(typeof(*(var)), field)
#endif

#define TMB_ADDRES_OF(T, x)                                                    \
    &(struct { TMB_TYPEOF(T) a; }) {                                           \
        x                                                                      \
    }
#define TMB_OFFSETOF_DEREF(__var, __field1, __field2)                          \
    ((size_t)&(((TMB_TYPEOF(*((TMB_TYPEOF(__var)*)0)->__field1)*)0)->__field2))

#define TMB_HM_DEFAULT_START_SIZE 8
#define _hm_header_(K, V)                                                      \
    int capacity;                                                              \
    int occupied;                                                              \
    enum tmb_hm_key_type__ key_type;                                           \
    struct {                                                                   \
        K key;                                                                 \
        V value;                                                               \
        bool occupied;                                                         \
    }* tmp;                                                                    \
    struct {                                                                   \
        K key;                                                                 \
        V value;                                                               \
        bool occupied;                                                         \
    }* buckets

typedef struct {
    _hm_header_(void*, void*);
} tmb_hash_map_internal;

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

#define ASSERT(expr)                                                           \
    do {                                                                       \
        if (!(expr)) {                                                         \
            UNUSED fprintf(                                                    \
                    stderr, "assertion failed at %s:%d", __FILE__, __LINE__);  \
            exit(1);                                                           \
        }                                                                      \
    } while (0)

#define UNUSED (void)

#define TMB_DA_INIT_CAP 16

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
        da_reserve(da, (da)->length + 1);                                      \
        (da)->items[(da)->length++] = (item);                                  \
    } while (0)

#define da_append_name(da, item, length_name, capacity_name, items_name)       \
    do {                                                                       \
        da_reserve_name(da, (da)->length_name + 1, capacity_name, items_name); \
        (da)->items_name[(da)->length_name++] = (item);                        \
    } while (0)

#define da_appendn(da, new_items_length, new_items)                            \
    do {                                                                       \
        da_reserve((da), (da)->length + (new_items_length));                   \
        memcpy((da)->items + (da)->length,                                     \
               (new_items),                                                    \
               (unsigned long)(new_items_length) * sizeof(*(da)->items));      \
        (da)->length += (new_items_length);                                    \
    } while (0)

#define da_last(da) (da)->items[ASSERT((da)->length > 0), (da)->length - 1]

#define da_for_each(T, it, da)                                                 \
    for (T * (it) = (da)->items; (it) < (da)->items + (da)->length; (it)++)

#define da_remove(da, idx)                                                     \
    do {                                                                       \
        size_t _m__i = (idx);                                                  \
        ASSERT(_m__i < (da)->length);                                          \
        (da)->items[j] = (da)->items[(da)->items[--(da)->length]]              \
    } while (0)

#define da_free(da)                                                            \
    do {                                                                       \
        if ((da)->capacity) {                                                  \
            free((da)->items);                                                 \
            (da)->capacity = 0;                                                \
            (da)->length   = 0;                                                \
            (da)->items    = NULL;                                             \
        }                                                                      \
    } while (0);

#define da_free_memb_fn(da, memb_data, memb_free_fn)                           \
    do {                                                                       \
        if ((da)->capacity) {                                                  \
            for (int i__m_ = 0; i__m_ < (da)->length; i__m_++) {               \
                memb_free_fn((da)->items[i__m_].memb_data);                    \
            }                                                                  \
            free((da)->items);                                                 \
            (da)->capacity = 0;                                                \
            (da)->length   = 0;                                                \
        }                                                                      \
    } while (0);

#define sb_to_cstr(sb) da_append(sb, 0)

#define sb_appendn(sb, n, buff) da_appendn(sb, n, buff)
#define sb_append(sb, chr)      da_append(sb, chr)
#define sb_free(sb)             da_free(sb)
#define sb_appendf(sb, fmt, ...)                                               \
    sb_appendf__(sb, fmt __VA_OPT__(, ) __VA_ARGS__)
#define sb_appendv(sb, fmt, args) sb_appendv__(sb, fmt, args)

#define sb_append_cstr(sb, str)                                                \
    do {                                                                       \
        const char* _m__s = (char*)(str);                                      \
        int _m__n         = (int)strlen(_m__s);                                \
        da_appendn(sb, _m__n, _m__s);                                          \
    } while (0)

#define sv_make(chrs)                                                          \
    (tmb_string_view_t) { .items = (chrs), .length = strlen(chrs) }

tmb_string_view_t sv_from_sb(tmb_string_builder_t* sb);
bool tmb_sv_cmp(tmb_string_view_t* sv1, tmb_string_view_t* sv2);

void tmb_hm_get_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        const void* addr_of_new_key,
                        size_t key_size,
                        size_t key_offset,
                        size_t occupied_offset);

void tmb_hm_del_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        const void* addr_of_new_key,
                        size_t key_size,
                        size_t key_offset,
                        size_t occupied_offset);

void tmb_hm_set_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        const void* addr_of_new_key,
                        size_t key_size,
                        size_t key_offset,
                        void* value,
                        size_t value_size,
                        size_t value_offset,
                        size_t occupied_offset);
#define hm_init(hm, opt)
#define hm_put(hm, new_key, new_value)                                         \
    do { /*TODO move this ifs to wrapper*/                                     \
        if ((hm)->key_type == 0) {                                             \
            TMB_TYPEOF((hm)->tmp->key) a__m;                                   \
            (hm)->key_type = TMB_DISTINGUISH_HM_TYPE(a__m);                    \
        }                                                                      \
        if ((hm)->buckets == NULL) {                                           \
            (hm)->buckets  = calloc(TMB_HM_DEFAULT_START_SIZE,                 \
                                   sizeof(*(hm)->buckets));                   \
            (hm)->capacity = TMB_HM_DEFAULT_START_SIZE;                        \
        }                                                                      \
        tmb_hm_set_wrapper(hm,                                                 \
                           sizeof(*(hm)->buckets),                             \
                           TMB_OFFSETOF((hm), buckets),                        \
                           (void*)TMB_ADDRES_OF((hm)->tmp->key, new_key),      \
                           sizeof(TMB_TYPEOF((hm)->tmp->key)),                 \
                           TMB_OFFSETOF_DEREF(*(hm), tmp, key),                \
                           (void*)TMB_ADDRES_OF((hm)->tmp->value, new_value),  \
                           sizeof(TMB_TYPEOF((hm)->tmp->value)),               \
                           TMB_OFFSETOF_DEREF(*(hm), tmp, value),              \
                           TMB_OFFSETOF_DEREF(*(hm), tmp, occupied));          \
    } while (0)

#define hm_get(hm, _key)                                                       \
    (tmb_hm_get_wrapper(hm,                                                    \
                        sizeof(*(hm)->buckets),                                \
                        TMB_OFFSETOF((hm), buckets),                           \
                        (void*)TMB_ADDRES_OF((hm)->tmp->key, _key),            \
                        sizeof(TMB_TYPEOF((hm)->tmp->key)),                    \
                        TMB_OFFSETOF_DEREF(*(hm), tmp, key),                   \
                        TMB_OFFSETOF_DEREF(*(hm), tmp, occupied)),             \
     (hm)->tmp->value)

#define hm_exists(hm, _key)                                                    \
    (tmb_hm_get_wrapper(hm,                                                    \
                        sizeof(*(hm)->buckets),                                \
                        TMB_OFFSETOF((hm), buckets),                           \
                        (void*)TMB_ADDRES_OF((hm)->tmp->key, _key),            \
                        sizeof(TMB_TYPEOF((hm)->tmp->key)),                    \
                        TMB_OFFSETOF_DEREF(*(hm), tmp, key),                   \
                        TMB_OFFSETOF_DEREF(*(hm), tmp, occupied)),             \
     (hm)->tmp->occupied)

#define hm_del(hm, _key)                                                       \
    tmb_hm_del_wrapper(hm,                                                     \
                       sizeof(*(hm)->buckets),                                 \
                       TMB_OFFSETOF((hm), buckets),                            \
                       (void*)TMB_ADDRES_OF((hm)->tmp->key, _key),             \
                       sizeof(TMB_TYPEOF((hm)->tmp->key)),                     \
                       TMB_OFFSETOF_DEREF(*(hm), tmp, key),                    \
                       TMB_OFFSETOF_DEREF(*(hm), tmp, occupied))

void sb_appendf__(tmb_string_builder_t* sb, const char* fmt, ...)
        TMB_FMT_CHECK(2, 3);
void sb_appendv__(tmb_string_builder_t* sb, const char* fmt, va_list args);
void do_nothing__(void* _data);
void tmb_sb_just(tmb_string_builder_t* sb,
                 enum tmb_sb_just_opt just_setting,
                 int amount,
                 char pad_char);
void tmb_sb_truncate(tmb_string_builder_t* sb,
                     enum tmb_sb_truncate_opt truncate_setting,
                     int max_len);

bool tmb_is_substring(tmb_string_view_t haystack, const char* needle);

/**
 * @brief Returns heap allocated zero terminated string with contents of the
 * file
 *
 * @param file
 * @return char*
 */
char* load_entire_file(const char* file);
char* tmb_strdup(const char* const s);

#endif // TMB_LIB_H_
