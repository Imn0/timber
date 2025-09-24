#include <tmb_lib.h>

int main() {
    {
        tmb_string_view_t sv1 = sv_make("a");

        tmb_string_builder_t sb = { 0 };
        sb_append_cstr(&sb, "a");
        tmb_string_view_t sv2 = sv_from_sb(&sb);

        ASSERT(tmb_sv_cmp(&sv1, &sv2));
    }
    {
        tmb_string_view_t sv1 = sv_make("a");

        tmb_string_builder_t sb = { 0 };
        sb_append_cstr(&sb, "b");
        tmb_string_view_t sv2 = sv_from_sb(&sb);

        ASSERT(!tmb_sv_cmp(&sv1, &sv2));
    }
}
