#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <tmb_lib.h>

/*
[formats]
format_name="[ %date ] %message"
other_format="[ %date ] %message"
*/

typedef void fmt_fn(StringBuilder* sb, const LogCtx* ctx);
typedef void sink_fn(const char* msg, void* data);

typedef enum {
    FMT_STR,
    FMT_FN,
    FMT_MSG
} FormatterBlockKind;

typedef struct {
    int kind;
    union {
        char* str;
        fmt_fn* fmt_function;
    } d;
} FormatterBlock;

typedef struct {
    int block_count;
    FormatterBlock blocks[];
} Formatter;

char* fmt_format(Formatter* fmt,
                 const LogCtx* ctx,
                 const char* msg_fmt,
                 va_list msg_arg) {
    StringBuilder sb = { 0 };
    for (int i = 0; i < fmt->block_count; i++) {
        switch (fmt->blocks[i].kind) {
        case FMT_STR:
            sb_append_cstr(&sb, fmt->blocks[i].d.str);
            break;
        case FMT_FN:
            fmt->blocks[i].d.fmt_function(&sb, ctx);
            break;
        case FMT_MSG:
            sb_appendf(&sb, msg_fmt, msg_arg);
            break;
        default:
            UNREACHABLE("");
        }
    }
    sb_to_cstr(&sb);
    return sb.items;
}

void _append_filename(StringBuilder* sb, const LogCtx* ctx) {
    sb_appendf(sb, "%s", ctx->filename);
}

void _stderror_sink(const char* msg, void* data) {
    UNUSED data;
    fprintf(stderr, "%s", msg);
}

typedef struct {
    sink_fn* fn;
    void* sink_data;
} Sink;

void tmb_logger_init_default(Logger* lg) {
    lg->formatters = malloc(sizeof(void*) * 2);
    Formatter* fmt1 = malloc(sizeof(Formatter) + 4 * (sizeof(FormatterBlock)));
    fmt1->block_count = 4;
    fmt1->blocks[0] = (FormatterBlock) { .kind = FMT_STR, .d.str = "[ " };
    fmt1->blocks[1] = (FormatterBlock) { .kind = FMT_FN,
                                         .d.fmt_function = _append_filename };
    fmt1->blocks[2] = (FormatterBlock) { .kind = FMT_STR, .d.str = " ] " };
    fmt1->blocks[3] = (FormatterBlock) { .kind = FMT_MSG };

    lg->formatters[0] = fmt1;
    lg->formatters[1] = NULL;

    lg->sinks = malloc(sizeof(void*) * 2);
    lg->sinks[1] = NULL;
}

void __tmb_log(const Logger* logger,
               tmb_log_level log_level,
               LogCtx ctx,
               const char* message,
               ...) {
    UNUSED log_level;
    Formatter** fmts = (Formatter**)logger->formatters;
    int i = 0;
    while (fmts[i] != NULL) {
        Formatter* fmt = fmts[i];
        va_list args;
        va_start(args, message);
        printf("%s\n", fmt_format(fmt, &ctx, message, args));
        va_end(args);
        i++;
    }
}

void tmb_test(void) {
    printf("helloge\n");
}
