#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <tmb_lib.h>

/*
[formats]
format_name="[ %date ] %message"
other_format="[ %date ] %message"
*/

typedef void fmt_fn_t(StringBuilder* sb, const LogCtx* ctx);
typedef void sink_log_fn_t(const char* msg, void* data);
typedef void free_fn_t(void* data);

typedef enum {
    FMT_STR,
    FMT_FN,
    FMT_MSG
} FormatTokenType;

typedef struct {
    FormatTokenType type;
    union {
        char* str;
        fmt_fn_t* fmt_function;
    } data;
} FormatToken;

typedef struct {
    int token_count;
    FormatToken tokens[];
} Formatter;

typedef struct {
    sink_log_fn_t* sink_log;
    free_fn_t* free_fn;
    void* sink_data;
} Sink;

char* fmt_format(Formatter* fmt,
                 const LogCtx* ctx,
                 const char* msg_fmt,
                 va_list msg_arg) {
    StringBuilder sb = { 0 };
    for (int i = 0; i < fmt->token_count; i++) {
        switch (fmt->tokens[i].type) {
        case FMT_STR:
            sb_append_cstr(&sb, fmt->tokens[i].data.str);
            break;
        case FMT_FN:
            fmt->tokens[i].data.fmt_function(&sb, ctx);
            break;
        case FMT_MSG:
            sb_appendv(&sb, msg_fmt, msg_arg);
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

void __do_nothing(void* __data) {
    UNUSED __data;
}

void _stderror_sink(const char* msg, void* data) {
    UNUSED data;
    fprintf(stderr, "%s", msg);
}

bool tmb_logger_init_default(Logger* lg) {
    if (lg == NULL) { return false; }

    // allocate memory for sinks and formatters
    lg->formatters = malloc(sizeof(void*));
    lg->sinks = malloc(sizeof(void*));

    // create formatter
    lg->formatters_count = 1;
    Formatter* fmt1 = malloc(sizeof(Formatter) + 4 * (sizeof(FormatToken)));
    fmt1->token_count = 4;
    fmt1->tokens[0] = (FormatToken) { .type = FMT_STR, .data.str = "[ " };
    fmt1->tokens[1] = (FormatToken) { .type = FMT_FN,
                                      .data.fmt_function = _append_filename };
    fmt1->tokens[2] = (FormatToken) { .type = FMT_STR, .data.str = " ] " };
    fmt1->tokens[3] = (FormatToken) { .type = FMT_MSG };
    lg->formatters[0] = fmt1;

    // create sink
    lg->sinks_count = 1;
    Sink* sink = malloc(sizeof(Sink));
    sink->sink_data = NULL;
    sink->sink_log = _stderror_sink;
    sink->free_fn = __do_nothing;
    lg->sinks[0] = sink;

    // map sink to fmt
    lg->fmt_map = malloc(sizeof(int) * 1);
    lg->fmt_map[0] = 0;
    return true;
}

bool tmb_logger_destroy(Logger* lg) {
    for (int i = 0; i < lg->formatters_count; i++) { free(lg->formatters[i]); }
    for (int i = 0; i < lg->sinks_count; i++) {
        Sink* sink = lg->sinks[i];
        sink->free_fn(sink->sink_data);
        free(lg->sinks[i]);
    }

    free(lg->sinks);
    free(lg->formatters);
    free(lg->fmt_map);
    return true;
}

void tmb_log(const Logger* logger, LogCtx ctx, const char* message, ...) {

    char** formated = malloc(logger->formatters_count * sizeof(char*));
    Formatter** fmts = (Formatter**)logger->formatters;
    Sink** sinks = (Sink**)logger->sinks;
    for (int i = 0; i < logger->formatters_count; i++) {
        Formatter* fmt = fmts[i];
        assert(fmt != NULL);
        va_list args;
        va_start(args, message);
        formated[i] = fmt_format(fmt, &ctx, message, args);
        va_end(args);
    }
    for (int i = 0; i < logger->sinks_count; i++) {
        Sink* sink = sinks[i];
        assert(sink != NULL);
        sink->sink_log(formated[logger->fmt_map[i]], sink->sink_data);
    }
    for (int i = 0; i < logger->formatters_count; i++) { free(formated[i]); }
    free(formated);
}

void tmb_test(void) {
    printf("helloge\n");
}
