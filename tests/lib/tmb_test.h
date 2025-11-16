#ifndef TMB_TEST_H_
#define TMB_TEST_H_

#include <tmb_lib.h>

typedef void (*test_fn)(void);

typedef struct {
    int total;
    int passed;
    int failed;
} test_stats_t;

typedef struct {
    int line;
    char* file;
    tmb_string_builder_t message;
} assertion_fail_meta_t;

typedef struct test_case {
    const char* name;
    const char* suite_name;
    test_fn func;
    test_stats_t stats;
    struct {
        _da_header_(assertion_fail_meta_t*);
    } failed;
} test_case_t;

extern test_case_t* current_test;

typedef struct {
    _da_header_(test_case_t*);
} test_suite_t;

void register_test(const char* suite_name, const char* name, test_fn func);

#define PUSH_FAIL(sb)                                                          \
    do {                                                                       \
        current_test->stats.failed++;                                          \
        da_append(&current_test->failed,                                       \
                  ((assertion_fail_meta_t) { .line    = __LINE__,              \
                                             .file    = __FILE__,              \
                                             .message = (sb) }));              \
    } while (0)

#define ASSERT2(condition)                                                     \
    do {                                                                       \
        current_test->stats.total++;                                           \
        if (!(condition)) {                                                    \
            tmb_string_builder_t sb = { 0 };                                   \
            sb_appendf(&sb, "%s", #condition);                                 \
            sb_to_cstr(&sb);                                                   \
            PUSH_FAIL(sb);                                                     \
            return;                                                            \
        }                                                                      \
    } while (0)

#define ASSERT_EQ(a, b)                                                        \
    do {                                                                       \
        current_test->stats.total++;                                           \
        if ((a) != (b)) {                                                      \
            tmb_string_builder_t sb = { 0 };                                   \
            sb_appendf(                                                        \
                    &sb, "%s == %s (%d != %d)", #a, #b, (int)(a), (int)(b));   \
            sb_to_cstr(&sb);                                                   \
            PUSH_FAIL(sb);                                                     \
            return;                                                            \
        }                                                                      \
    } while (0)

#define ASSERT_STR_EQ(a, b)                                                    \
    do {                                                                       \
        current_test->stats.total++;                                           \
        if (strcmp((a), (b)) != 0) {                                           \
            tmb_string_builder_t sb = { 0 };                                   \
            sb_appendf(&sb, "%s == %s (\"%s\" != \"%s\")", #a, #b, (a), (b));  \
            sb_to_cstr(&sb);                                                   \
            PUSH_FAIL(sb);                                                     \
            return;                                                            \
        }                                                                      \
    } while (0)

#ifdef _MSC_VER
    // MSVC: Use #pragma section and __declspec(allocate)
    #pragma section(".CRT$XCU", read)
    #define TEST(suite_name, name)                                             \
        static void test_##suite_name##_##name(void);                          \
        static void register_##suite_name##_##name(void) {                     \
            register_test(#suite_name, #name, test_##suite_name##_##name);     \
        }                                                                      \
        __declspec(allocate(".CRT$XCU"))                                       \
        static void (*suite_name##_##name##_auto_register)(void) =             \
                register_##suite_name##_##name;                                \
        static void test_##suite_name##_##name(void)
#else
    #define TEST(suite_name, name)                                             \
        static void test_##suite_name##_##name(void);                          \
        static void __attribute__((constructor))                               \
                register_##suite_name##_##name(void) {                         \
            register_test(#suite_name, #name, test_##suite_name##_##name);     \
        }                                                                      \
        static void test_##suite_name##_##name(void)
#endif

#endif // TMB_TEST_H_