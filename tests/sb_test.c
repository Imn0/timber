#include <assert.h>
#include <string.h>
#include <tmb_lib.h>

int main(void) {
    StringBuilder sb = { 0 };
    sb_append_cstr(&sb, "hello");
    sb_append_cstr(&sb, " world");
    assert(sb.size == 11); // no \0 termination
    assert(strncmp("hello world", sb.items, sb.size) == 0);

    sb_appendf(&sb, "%d", 3);
    assert(sb.size == 12);
    assert(strncmp("hello world3", sb.items, sb.size) == 0);
}
