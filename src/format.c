#include <format.h>

void const_format_token_fn(StringBuilder* sb, const LogCtx* ctx, void* data) {
    UNUSED ctx;
    String* _data = (String*)data;
    sb_appendn(sb, _data->items, _data->size);
}

void const_format_token_init(FormatToken* fmt, const cstr value) {
    fmt->type = FMT_FN;
    fmt->fmt_function = const_format_token_fn;

    fmt->token_data = make_string(value, strlen(value));
    fmt->free_fn = free;
}

void filename_format_token_fn(StringBuilder* sb,
                              const LogCtx* ctx,
                              void* data) {
    UNUSED data;
    sb_appendn(sb, ctx->filename, ctx->filename_len);
}

void filename_format_token_init(FormatToken* fmt) {
    fmt->token_data = NULL;
    fmt->fmt_function = filename_format_token_fn;
    fmt->free_fn = do_nothing;
    fmt->type = FMT_FN;
}

void log_level_format_color_fn(StringBuilder* sb,
                               const LogCtx* ctx,
                               void* data) {
    sb_appendn(sb,
               tmb_log_level_str[ctx->log_level],
               tmb_log_level_str_len[ctx->log_level]);
}
void log_level_format_fn(StringBuilder* sb, const LogCtx* ctx, void* data) {
    sb_appendn(sb,
               tmb_log_level_str[ctx->log_level],
               tmb_log_level_str_len[ctx->log_level]);
}

void log_level_format_init(FormatToken* fmt, bool use_color) {
    fmt->token_data = NULL;
    fmt->fmt_function = use_color ? log_level_format_color_fn
                                  : log_level_format_fn;
    fmt->free_fn = do_nothing;
    fmt->type = FMT_FN;
}
