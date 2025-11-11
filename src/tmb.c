#include <formatter.h>
#include <tmb_internal.h>

#include <tmb/sink.h>

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

const char* const TMB_PATCH_V    = "0";
const char* const TMB_MINOR_V    = "0";
const char* const TMB_MAJOR_V    = "0";
const char* const TMB_SO_V       = "0";
const char* const TMB_BUILD_TIME = __DATE__ ", " __TIME__;

tmb_logger_registry_t tmb_logger_registry = { 0 };

const tmb_cfg_t tmb_default_logger_cfg = { TMB_DEFAULT_LOGGER_CFG };
tmb_cfg_t tmb_cfg                      = { TMB_DEFAULT_LIB_CFG };

const char* const tmb_log_level_str[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_LEVEL_FATAL_STR,
    [TMB_LEVEL_ERROR]   = TMB_LEVEL_ERROR_STR,
    [TMB_LEVEL_WARNING] = TMB_LEVEL_WARNING_STR,
    [TMB_LEVEL_INFO]    = TMB_LEVEL_INFO_STR,
    [TMB_LEVEL_DEBUG]   = TMB_LEVEL_DEBUG_STR,
    [TMB_LEVEL_TRACE]   = TMB_LEVEL_TRACE_STR,
};

const int tmb_log_level_str_len[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_CONST_STR_SIZE(TMB_LEVEL_FATAL_STR),
    [TMB_LEVEL_ERROR]   = TMB_CONST_STR_SIZE(TMB_LEVEL_ERROR_STR),
    [TMB_LEVEL_WARNING] = TMB_CONST_STR_SIZE(TMB_LEVEL_WARNING_STR),
    [TMB_LEVEL_INFO]    = TMB_CONST_STR_SIZE(TMB_LEVEL_INFO_STR),
    [TMB_LEVEL_DEBUG]   = TMB_CONST_STR_SIZE(TMB_LEVEL_DEBUG_STR),
    [TMB_LEVEL_TRACE]   = TMB_CONST_STR_SIZE(TMB_LEVEL_TRACE_STR),
};

const char* const tmb_log_level_color[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_LEVEL_FATAL_COLOR,
    [TMB_LEVEL_ERROR]   = TMB_LEVEL_ERROR_COLOR,
    [TMB_LEVEL_WARNING] = TMB_LEVEL_WARNING_COLOR,
    [TMB_LEVEL_INFO]    = TMB_LEVEL_INFO_COLOR,
    [TMB_LEVEL_DEBUG]   = TMB_LEVEL_DEBUG_COLOR,
    [TMB_LEVEL_TRACE]   = TMB_LEVEL_TRACE_COLOR,
};

const int tmb_log_level_color_len[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_CONST_STR_SIZE(TMB_LEVEL_FATAL_COLOR),
    [TMB_LEVEL_ERROR]   = TMB_CONST_STR_SIZE(TMB_LEVEL_ERROR_COLOR),
    [TMB_LEVEL_WARNING] = TMB_CONST_STR_SIZE(TMB_LEVEL_WARNING_COLOR),
    [TMB_LEVEL_INFO]    = TMB_CONST_STR_SIZE(TMB_LEVEL_INFO_COLOR),
    [TMB_LEVEL_DEBUG]   = TMB_CONST_STR_SIZE(TMB_LEVEL_DEBUG_COLOR),
    [TMB_LEVEL_TRACE]   = TMB_CONST_STR_SIZE(TMB_LEVEL_TRACE_COLOR),
};

const char tmb_log_level_char[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL] = 'F', [TMB_LEVEL_ERROR] = 'E', [TMB_LEVEL_WARNING] = 'W',
    [TMB_LEVEL_INFO] = 'I',  [TMB_LEVEL_DEBUG] = 'D', [TMB_LEVEL_TRACE] = 'T',
};

static tmb_logger_t default_logger     = { 0 };
static bool default_logger_initialized = false;

void tmb_print_version(void) {
    printf("%s.%s.%s @ %s\nSO Version: %s\n"
           "Build %s\n",
           TMB_MAJOR_V,
           TMB_MINOR_V,
           TMB_PATCH_V,
           TMB_GIT_REV,
           TMB_SO_V,
           TMB_BUILD_TIME);
}

const char* tmb_get_version(void) {
    static tmb_string_builder_t sb = { 0 };
    if (sb.length == 0) {
        sb_appendf(&sb,
                   "%s.%s.%s @ %s\nSO Version: %s\n"
                   "Build %s\n",
                   TMB_MAJOR_V,
                   TMB_MINOR_V,
                   TMB_PATCH_V,
                   TMB_GIT_REV,
                   TMB_SO_V,
                   TMB_BUILD_TIME);
        sb_to_cstr(&sb);
    }
    return sb.items;
}

tmb_logger_t* tmb_get_default_logger() {
    if (!default_logger_initialized) {
        tmb_logger_set_default_format(&default_logger, TMB_DEFAULT_FORMAT);
        tmb_logger_add_sink(&default_logger, TMB_SINK_STDERR());
        default_logger.cfg         = tmb_default_logger_cfg;
        default_logger_initialized = true;
    }
    return &default_logger;
}

void tmb_register_logger(const char* name, tmb_logger_t* logger) {
    hm_put(&tmb_logger_registry, name, logger);
}

tmb_logger_t* tmb_get_logger(const char* name) {
    return hm_get(&tmb_logger_registry, name);
}

tmb_logger_t* tmb_get_logger_or_default(const char* name) {
    tmb_logger_t* lgr = hm_get(&tmb_logger_registry, name);
    if (lgr == NULL) { lgr = tmb_get_default_logger(); }
    return lgr;
}

void tmb_set_global_options(tmb_cfg_t cfg) {
    tmb_cfg = cfg;
}

/// LOGGING FUNCTIONS
static inline void tmb_log_impl_ext_ctx__(tmb_log_ctx_t ctx,
                                          tmb_logger_t* logger) {
    if (ctx.log_level > tmb_cfg.log_level) { return; }
    if (ctx.log_level > logger->cfg.log_level) { return; }

    if (logger->formatters.length < 1) {
        tmb_logger_set_default_format(logger, TMB_DEFAULT_FORMAT);
    }
    struct {
        _da_header_(tmb_formatted_msg_t*);
    } formatted_messages = { 0 };

    for (int j = 0; j < logger->formatters.length; j++) {
        tmb_formatter_t* formatter = &logger->formatters.items[j];
        da_append(&formatted_messages,
                  formatter->format_fn(formatter, &ctx, logger));
    }

    for (int i = 0; i < logger->sinks.length; i++) {
        int fmt_idx = logger->sink_formatter_map.items[i];
        logger->sinks.items[i]->sink_fn(
                formatted_messages.items[fmt_idx].items,
                formatted_messages.items[fmt_idx].length,
                logger->sinks.items[i]->sink_data);
    }
    if (logger->sinks.length < 1) {
        printf("%.*s",
               formatted_messages.items[0].length,
               formatted_messages.items[0].items);
    }

    for (int j = 0; j < logger->formatters.length; j++) {
        tmb_formatter_t* formatter = &logger->formatters.items[j];
        formatter->formated_free_fn(formatted_messages.items[j].message);
    }
    da_free(&formatted_messages);
}

static inline void tmb_log_impl__(tmb_log_ctx_t ctx,
                                  tmb_logger_t* logger,
                                  const char* message,
                                  va_list args) {
    tmb_time_stamp_t stop_watch = { 0 };
    tmb_time_stamp_t ts         = { 0 };

    if (logger->has.time_stamp) { ts = tmb_time_stamp(); }
    if (logger->has.stopwatch) { stop_watch = tmb_time_stopwatch(); }

    tmb_string_builder_t message_filled = { 0 };
    sb_appendv(&message_filled, message, args);

    ctx.message        = message_filled.items;
    ctx.message_len    = message_filled.length;
    ctx.ts_nsec        = ts.nsec;
    ctx.ts_sec         = ts.sec;
    ctx.stopwatch_sec  = stop_watch.sec;
    ctx.stopwatch_nsec = stop_watch.nsec;

    tmb_log_impl_ext_ctx__(ctx, logger);
    if (logger->has.time_stamp)
        logger->last_message_stopwatch_nsec = stop_watch.nsec;
    if (logger->has.time_stamp)
        logger->last_message_stopwatch_sec = stop_watch.sec;
    sb_free(&message_filled);
}

void tmb_log(tmb_log_ctx_t ctx,
             tmb_logger_t* logger,
             const char* message,
             ...) {
    va_list args;
    va_start(args, message);
    tmb_log_impl__(ctx, logger, message, args);
    va_end(args);
}

void tmb_log_default(tmb_log_ctx_t ctx, const char* message, ...) {
    va_list args;
    va_start(args, message);
    tmb_log_impl__(ctx, tmb_get_default_logger(), message, args);
    va_end(args);
}
