#include <tmb_internal.h>

tmb_formatter_t tmb_formatter_json_make(tmb_json_formatter_opts_t opts) {
    tmb_json_formatter_opts_t* malloced_opts = malloc(sizeof(*malloced_opts));

    tmb_string_builder_t sb = { 0 };
    sb_append_cstr(&sb, "{{");
    if (opts.has_stopwatch) sb_append_cstr(&sb, "\"last_message\":\"{d}\",");
    if (opts.has_timestamp) sb_append_cstr(&sb, "\"timestamp\":\"{D}\",");
    sb_append_cstr(&sb, "\"log_level\":\"{l}\",");
    sb_append_cstr(&sb, "\"file\":\"{@}\",");
    sb_append_cstr(&sb, "\"line_no\":\"{#}\",");
    sb_append_cstr(&sb, "\"function\":\"{f}\",");
    sb_append_cstr(&sb, "\"message\":\"{$}\"");
    sb_append_cstr(&sb, "}}\n");
    sb_to_cstr(&sb);

    tmb_formatter_t fmt = tmb_formatter_make(sb.items);
    fmt.enable.colors   = false;
    return fmt;
}
