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
               enum tmb_sb_just_opt how,
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
    tmb_sb_just(&sb, how, amount, ' ');

    CHECK(tmb_sv_cmp(&sv_make(expected), &sb_as_sv(&sb)));
}

int main(void) {
    TEST_CASE("h", 1, JUST_CENTER, "h");
    TEST_CASE("h", 1, JUST_LEFT, "h");
    TEST_CASE("h", 1, JUST_RIGHT, "h");
    TEST_CASE("hh", 1, JUST_CENTER, "hh");
    TEST_CASE("hh", 1, JUST_LEFT, "hh");
    TEST_CASE("hh", 1, JUST_RIGHT, "hh");

    TEST_CASE("h", 2, JUST_RIGHT, " h");
    TEST_CASE("h", 2, JUST_LEFT, "h ");
    TEST_CASE("h", 2, JUST_CENTER, "h ");

    TEST_CASE("h", 3, JUST_RIGHT, "  h");
    TEST_CASE("h", 3, JUST_LEFT, "h  ");
    TEST_CASE("h", 3, JUST_CENTER, " h ");
    TEST_CASE("hh", 3, JUST_CENTER, "hh ");
    TEST_CASE("hh", 5, JUST_CENTER, " hh  ");
    TEST_CASE("hh", 4, JUST_CENTER, " hh ");
    TEST_CASE("hello", 7, JUST_CENTER, " hello ");

    return ret;
}
