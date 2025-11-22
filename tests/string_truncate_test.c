#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <tmb_lib.h>

static int ret = 0;

#define TEST_CASE(what, amount, how, expected)                                 \
    test_just(__FILE__, __LINE__, what, amount, how, expected)

void test_just(const char* file,
               int line_no,
               const char* what,
               int amount,
               enum tmb_sb_truncate_opt how,
               const char* expected) {
#define CHECK(v)                                                               \
    do {                                                                       \
        if (!(v)) {                                                            \
            printf("%s:%d\n", file, line_no);                                  \
            printf("expected: >%s<\ngot: >%.*s<\n\tamount:%d\n\thow:%d\n",     \
                   expected,                                                   \
                   sb.length,                                                  \
                   sb.items,                                                   \
                   amount,                                                     \
                   how);                                                       \
            ret |= 1;                                                          \
        }                                                                      \
    } while (0)

    tmb_string_builder_t sb = { 0 };
    sb_append_cstr(&sb, what);
    tmb_sb_truncate(&sb, how, amount);

    CHECK(tmb_sv_cmp(&sv_make(expected), &sb_as_sv(&sb)) == 0);
}

int main(void) {
    TEST_CASE("1234", 2, TRUNCATE_LEFT, "34");    // '['
    TEST_CASE("1234", 4, TRUNCATE_LEFT, "1234");  // '['
    TEST_CASE("1234", 2, TRUNCATE_RIGHT, "12");   // ']'
    TEST_CASE("1234", 4, TRUNCATE_RIGHT, "1234"); // ']'

    return ret;
}
