#include "config/lex.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <config/lex.h>
#include <format.h>
#include <sink.h>
#include <string.h>
#include <tmb_lib.h>

const char* const tmb_log_level_str[] = {
    [TMB_LOG_TRACE] = "TRACE",     [TMB_LOG_DEBUG] = "DEBUG",
    [TMB_LOG_INFO] = "INFO",       [TMB_LOG_NOTICE] = "NOTICE",
    [TMB_LOG_WARNING] = "WARNING", [TMB_LOG_ERROR] = "ERROR",
    [TMB_LOG_ALERT] = "ALERT",     [TMB_LOG_CRITICAL] = "CRITICAL",
};

const int tmb_log_level_str_len[] = {
    [TMB_LOG_TRACE] = sizeof(tmb_log_level_str[TMB_LOG_TRACE]) - 1,
    [TMB_LOG_DEBUG] = sizeof(tmb_log_level_str[TMB_LOG_DEBUG]) - 1,
    [TMB_LOG_INFO] = sizeof(tmb_log_level_str[TMB_LOG_INFO]) - 1,
    [TMB_LOG_NOTICE] = sizeof(tmb_log_level_str[TMB_LOG_NOTICE]) - 1,
    [TMB_LOG_WARNING] = sizeof(tmb_log_level_str[TMB_LOG_WARNING]) - 1,
    [TMB_LOG_ERROR] = sizeof(tmb_log_level_str[TMB_LOG_ERROR]) - 1,
    [TMB_LOG_ALERT] = sizeof(tmb_log_level_str[TMB_LOG_ALERT]) - 1,
    [TMB_LOG_CRITICAL] = sizeof(tmb_log_level_str[TMB_LOG_CRITICAL]) - 1,
};

cstr fmt_format(Formatter* fmt,
                const LogCtx* ctx,
                const cstr msg_fmt,
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

bool tmb_logger_init(Logger* lgr, const char* config) {
    UNUSED lgr;
    Lexer lex;
    lexer_init(&lex, config, strlen(config));

    Token tok = { 0 };
    while (true) {
        lexer_lex(&lex, &tok);
        token_print(&tok);
        if (tok.type == TOK_EOF) break;
    }

    return true;
}

bool tmb_logger_init_file(Logger* lgr, const char* filename) {
    char* config = load_entire_file(filename);
    tmb_logger_init(lgr, config);
    return true;
}

bool tmb_logger_init_default(Logger* lg) {
    if (lg == NULL) { return false; }

    // allocate memory for sinks and formatters
    lg->formatters = (void**)malloc(sizeof(void*));
    lg->sinks = (void**)malloc(sizeof(void*));

    // create formatter
    lg->formatters_count = 1;
    const int default_token_count = 5;
    Formatter* fmt1 = malloc(sizeof(Formatter) +
                             default_token_count * (sizeof(FormatToken)));
    lg->formatters[0] = fmt1;

    fmt1->token_count = default_token_count;

    const_format_token_init(&fmt1->tokens[0], "[ ");
    log_level_format_init(&fmt1->tokens[1], false);
    const_format_token_init(&fmt1->tokens[2], " ] ");
    fmt1->tokens[3] = (FormatToken) { .type = FMT_MSG };
    const_format_token_init(&fmt1->tokens[4], TMB_NEW_LINE);

    // create sink
    lg->sinks_count = 1;
    lg->sinks[0] = stderror_sink_create();

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

    free((void*)lg->sinks);
    free((void*)lg->formatters);
    free(lg->fmt_map);
    return true;
}

void tmb_log(const Logger* logger, LogCtx ctx, const char* message, ...) {
    cstr* formated = (cstr*)malloc(logger->formatters_count * sizeof(cstr));
    Formatter** fmts = (Formatter**)logger->formatters;
    Sink** sinks = (Sink**)logger->sinks;
    for (int i = 0; i < logger->formatters_count; i++) {
        Formatter* fmt = fmts[i];
        assert(fmt != NULL);
        va_list args = NULL;
        va_start(args, message);
        formated[i] = fmt_format(fmt, &ctx, message, args);
        va_end(args);
    }
    for (int i = 0; i < logger->sinks_count; i++) {
        Sink* sink = sinks[i];
        assert(sink != NULL);
        sink->sink_log(formated[logger->fmt_map[i]], sink->sink_data);
    }
    for (int i = 0; i < logger->formatters_count; i++) {
        free((void*)formated[i]);
    }
    free((void*)formated);
}

void tmb_print_version(void) {
    printf("%s.%s.%s @ %s\nSO Version: %s\n",
           TMB_PATCH_V,
           TMB_MINOR_V,
           TMB_MAJOR_V,
           GIT_REV,
           TMB_SO_V);
}
