#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <tmb_lib.h>

void test_sb_anppendv(StringBuilder* sb, char* msg, ...) {
    va_list args;
    va_start(args, msg);
    sb_appendv(sb, msg, args);
    va_end(args);
}

int main(void) {
    StringBuilder sb = { 0 };
    sb_append_cstr(&sb, "hello");
    sb_append_cstr(&sb, " world");
    assert(sb.size == 11); // no \0 termination
    assert(strncmp("hello world", sb.items, sb.size) == 0);

    sb_appendf(&sb, "%d", 3);
    assert(sb.size == 12);
    assert(strncmp("hello world3", sb.items, sb.size) == 0);

    test_sb_anppendv(&sb, "aa%d", 2);
    assert(sb.size == 15);
    assert(strncmp("hello world3aa2", sb.items, sb.size) == 0);

    sb_appendn(&sb, "hell", 4);
    assert(sb.size == 19);
    assert(strncmp("hello world3aa2hell", sb.items, sb.size) == 0);
}
