#include <time.h>

#include <tmb.h>
#include <tmb_lib.h>

void filename_format_token_fn(StringBuilder* sb,
                              const LogCtx* ctx,
                              void* data) {
    UNUSED data;
    sb_appendn(sb, ctx->filename, ctx->filename_len);
}

void log_level_format_color_fn(StringBuilder* sb,
                               const LogCtx* ctx,
                               void* data) {
    UNUSED data;
    switch (ctx->log_level) {
    case TMB_LOG_LEVEL_FATAL:
        sb_append_cstr(sb, MAKE_ANSI(ANSI_RED));
        break;
    case TMB_LOG_LEVEL_ERROR:
        sb_append_cstr(sb, MAKE_ANSI(ANSI_RED));
        break;
    case TMB_LOG_LEVEL_WARNING:
        sb_append_cstr(sb, MAKE_ANSI(ANSI_YELLOW));
        break;
    case TMB_LOG_LEVEL_INFO:
        sb_append_cstr(sb, MAKE_ANSI(ANSI_GREEN));
        break;
    case TMB_LOG_LEVEL_DEBUG:
        sb_append_cstr(sb, MAKE_ANSI(ANSI_BLACK, ANSI_DIM));
        break;
    case TMB_LOG_LEVEL_TRACE:
        sb_append_cstr(sb, MAKE_ANSI(ANSI_BLACK, ANSI_DIM, ANSI_ITALIC));
        break;
    case TMB_LOG_LEVEL_COUNT:
    default:
        break;
    }
    sb_appendn(sb,
               tmb_log_level_str[ctx->log_level],
               tmb_log_level_str_len[ctx->log_level]);
    sb_append_cstr(sb, MAKE_ANSI(ANSI_RESET));
}

void log_level_format_fn(StringBuilder* sb, const LogCtx* ctx, void* data) {
    UNUSED data;
    sb_appendn(sb,
               tmb_log_level_str[ctx->log_level],
               tmb_log_level_str_len[ctx->log_level]);
}

void date_format_fn(StringBuilder* sb, const LogCtx* ctx, void* data) {
    UNUSED data;
    struct tm tm = *localtime(&ctx->log_time);
    sb_appendf(sb,
               "%d-%02d-%02d %02d:%02d:%02d",
               tm.tm_year + 1900,
               tm.tm_mon + 1,
               tm.tm_mday,
               tm.tm_hour,
               tm.tm_min,
               tm.tm_sec);
}

void file_format_fn(StringBuilder* sb, const LogCtx* ctx, void* data) {
    UNUSED data;
    sb_appendn(sb, ctx->filename, ctx->filename_len);
}

void function_format_fn(StringBuilder* sb, const LogCtx* ctx, void* data) {
    UNUSED data;
    sb_appendn(sb, ctx->funcname, ctx->funcname_len);
}

void tmb_formatter_add_chip(Formatter* fmt, FormatToken chip) {
    da_append(fmt, chip);
}

void tmb_formatter_add_chips(Formatter* fmt, size_t count, ...) {
    va_list args;
    va_start(args, count);
    da_reserve(fmt, fmt->size + count);
    for (size_t i = 0; i < count; i++) {
        da_append(fmt, va_arg(args, FormatToken));
    }
    va_end(args);
}

String tmb_formatter_format(Formatter* fmt,
                  const LogCtx* ctx,
                  const char* msg_fmt,
                  va_list msg_arg) {
    StringBuilder sb = { 0 };

    da_for_each(FormatToken, tok, fmt) {
        switch (tok->type) {
        case FMT_FN:
            tok->fmt_function(&sb, ctx, tok->token_data);
            break;
        case FMT_MSG:
            sb_appendv(&sb, msg_fmt, msg_arg);
            break;
        default:
            UNREACHABLE();
        }
    }
    return (String) { .items = sb.items, .size = sb.size };
}

struct const_fmt_data {
    size_t size;
    char items[];
};

void const_format_token_fn(StringBuilder* sb, const LogCtx* ctx, void* data) {
    UNUSED ctx;
    struct const_fmt_data* _data = (struct const_fmt_data*)data;
    sb_appendn(sb, _data->items, _data->size);
}

FormatToken tmb_fmt_chip_const_val_make(const char* value) {

    size_t value_size             = strlen(value);
    struct const_fmt_data* string = malloc(sizeof(struct const_fmt_data) +
                                           value_size * sizeof(char));

    string->size                  = value_size;
    memcpy(string->items, value, value_size);

    return (FormatToken) { .type         = FMT_FN,
                           .fmt_function = const_format_token_fn,
                           .token_data   = string,
                           .free_fn      = free };
}

FormatToken tmb_fmt_chip_message_make() {
    return (FormatToken) { .type = FMT_MSG, .free_fn = do_nothing };
}

FormatToken tmb_fmt_chip_log_level_make(bool use_color) {
    return (FormatToken) {
        .token_data   = NULL,
        .fmt_function = use_color ? log_level_format_color_fn
                                  : log_level_format_fn,
        .free_fn      = do_nothing,
        .type         = FMT_FN,
    };
}

FormatToken tmb_fmt_chip_funcname_make() {
    return (FormatToken) { .type         = FMT_FN,
                           .fmt_function = function_format_fn,
                           .token_data   = NULL,
                           .free_fn      = do_nothing };
}

FormatToken tmb_fmt_chip_filename_make() {
    return (FormatToken) { .type         = FMT_FN,
                           .fmt_function = file_format_fn,
                           .token_data   = NULL,
                           .free_fn      = do_nothing };
}

FormatToken tmb_fmt_chip_date_make() {
    return (FormatToken) { .type         = FMT_FN,
                           .fmt_function = date_format_fn,
                           .token_data   = NULL,
                           .free_fn      = do_nothing };
}
