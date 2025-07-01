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

typedef void fmt_fn_t(StringBuilder* sb, const LogCtx* ctx, void* data);
typedef void sink_log_fn_t(const char* msg, void* data);
typedef void free_fn_t(void* data);

typedef enum {
    FMT_FN,
    FMT_MSG
} FormatTokenType;

typedef struct {
    FormatTokenType type;
    void* token_data;
    fmt_fn_t* fmt_function;
    free_fn_t* free_fn;
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
        FormatToken* tok = &fmt->tokens[i];
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
    sb_to_cstr(&sb);
    return sb.items;
}

struct __fmt_fn_const_data {
    int str_len;
    char str[];
};
struct __fmt_fn_const_data* __fmt_fn_const_data_create(const cstr str) {
    int str_size = strlen(str);
    struct __fmt_fn_const_data* data = malloc(
            sizeof(struct __fmt_fn_const_data) + str_size * sizeof(char));
    strcpy(data->str, str);
    data->str_len = str_size;
    return data;
}
void __fmt_fn_const(StringBuilder* sb, const LogCtx* ctx, void* data) {
    struct __fmt_fn_const_data* _data = (struct __fmt_fn_const_data*)data;
    sb_appendn(sb, _data->str, _data->str_len);
}

void __fmt_fn_filename(StringBuilder* sb, const LogCtx* ctx, void* data) {
    sb_appendn(sb, ctx->filename, ctx->filename_len);
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
    fmt1->tokens[0] = (FormatToken) { .type = FMT_FN,
                                      .fmt_function = __fmt_fn_const,
                                      .token_data = __fmt_fn_const_data_create(
                                              "[ "),
                                      .free_fn = free };
    fmt1->tokens[1] = (FormatToken) { .type = FMT_FN,
                                      .fmt_function = __fmt_fn_filename };
    fmt1->tokens[2] = (FormatToken) { .type = FMT_FN,
                                      .fmt_function = __fmt_fn_const,
                                      .token_data = __fmt_fn_const_data_create(
                                              " ] "),
                                      .free_fn = free };
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
    for (int i = 0; i < lg->formatters_count; i++) {
        Formatter* fmt = lg->formatters[i];
        for (int j = 0; j < fmt->token_count; j++) {
            FormatToken* tok = &fmt->tokens[j];
            if (tok->free_fn) { tok->free_fn(tok->token_data); }
        }
        free(lg->formatters[i]);
    }
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

    cstr* formated = malloc(logger->formatters_count * sizeof(cstr));
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

void tmb_print_version(void) {
    printf("%s.%s.%s @ %s\nSO Version: %s\n",
           TMB_PATCH_V,
           TMB_MINOR_V,
           TMB_MAJOR_V,
           GIT_REV,
           TMB_SO_V);
}
