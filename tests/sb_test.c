#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <tmb_lib.h>

static void test_sb_anppendv(tmb_string_builder_t* sb, char* msg, ...) {
    va_list args;
    va_start(args, msg);
    sb_appendv(sb, msg, args);
    va_end(args);
}

int main(void) {
    tmb_string_builder_t sb = { 0 };
    sb_append_cstr(&sb, "hello");
    sb_append_cstr(&sb, " world");
    ASSERT(sb.length == 11); // no \0 termination
    ASSERT(strncmp("hello world", sb.items, sb.length) == 0);

    sb_appendf(&sb, "%d", 3);
    ASSERT(sb.length == 12);
    ASSERT(strncmp("hello world3", sb.items, sb.length) == 0);

    test_sb_anppendv(&sb, "aa%d", 2);
    ASSERT(sb.length == 15);
    ASSERT(strncmp("hello world3aa2", sb.items, sb.length) == 0);

    sb_appendn(&sb, 4, "hell");
    ASSERT(sb.length == 19);
    ASSERT(strncmp("hello world3aa2hell", sb.items, sb.length) == 0);
}
