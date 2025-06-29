#include <assert.h>
#include <tmb_lib.h>
#include <string.h>

int main() {
    StringBuilder sb = { 0 };
    sb_append_cstr(&sb, "hello");
    sb_append_cstr(&sb, " world");
    assert(strcmp("hello world", sb.items) == 0);
};
