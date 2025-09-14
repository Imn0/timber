#include <formatter.h>
#include <tmb_internal.h>

#include <tmb/sink.h>

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

const char* const TMB_PATCH_V = "0";
const char* const TMB_MINOR_V = "0";
const char* const TMB_MAJOR_V = "0";
const char* const TMB_SO_V    = "0";


tmb_logger_registry_t tmb_logger_registry = { 0 };

#define TMB_DEFAULT_CFG { .enable_colors = true, .max_log_level = LOG_LEVEL_INFO }
const tmb_cfg_t tmb_default_cfg           = TMB_DEFAULT_CFG;
tmb_cfg_t tmb_cfg                         = TMB_DEFAULT_CFG;
#undef TMB_DEFAULT_CFG

const char* const tmb_log_level_str[LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_LEVEL_FATAL_STR,
    [TMB_LEVEL_ERROR]   = TMB_LEVEL_ERROR_STR,
    [TMB_LEVEL_WARNING] = TMB_LEVEL_WARNING_STR,
    [TMB_LEVEL_INFO]    = TMB_LEVEL_INFO_STR,
    [TMB_LEVEL_DEBUG]   = TMB_LEVEL_DEBUG_STR,
    [TMB_LEVEL_TRACE]   = TMB_LEVEL_TRACE_STR,
};

const int tmb_log_level_str_len[LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_CONST_STR_SIZE(TMB_LEVEL_FATAL_STR),
    [TMB_LEVEL_ERROR]   = TMB_CONST_STR_SIZE(TMB_LEVEL_ERROR_STR),
    [TMB_LEVEL_WARNING] = TMB_CONST_STR_SIZE(TMB_LEVEL_WARNING_STR),
    [TMB_LEVEL_INFO]    = TMB_CONST_STR_SIZE(TMB_LEVEL_INFO_STR),
    [TMB_LEVEL_DEBUG]   = TMB_CONST_STR_SIZE(TMB_LEVEL_DEBUG_STR),
    [TMB_LEVEL_TRACE]   = TMB_CONST_STR_SIZE(TMB_LEVEL_TRACE_STR),
};

const char* const tmb_log_level_color[LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_LEVEL_FATAL_COLOR,
    [TMB_LEVEL_ERROR]   = TMB_LEVEL_ERROR_COLOR,
    [TMB_LEVEL_WARNING] = TMB_LEVEL_WARNING_COLOR,
    [TMB_LEVEL_INFO]    = TMB_LEVEL_INFO_COLOR,
    [TMB_LEVEL_DEBUG]   = TMB_LEVEL_DEBUG_COLOR,
    [TMB_LEVEL_TRACE]   = TMB_LEVEL_TRACE_COLOR,
};

const int tmb_log_level_color_len[LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_CONST_STR_SIZE(TMB_LEVEL_FATAL_COLOR),
    [TMB_LEVEL_ERROR]   = TMB_CONST_STR_SIZE(TMB_LEVEL_ERROR_COLOR),
    [TMB_LEVEL_WARNING] = TMB_CONST_STR_SIZE(TMB_LEVEL_WARNING_COLOR),
    [TMB_LEVEL_INFO]    = TMB_CONST_STR_SIZE(TMB_LEVEL_INFO_COLOR),
    [TMB_LEVEL_DEBUG]   = TMB_CONST_STR_SIZE(TMB_LEVEL_DEBUG_COLOR),
    [TMB_LEVEL_TRACE]   = TMB_CONST_STR_SIZE(TMB_LEVEL_TRACE_COLOR),
};

const char tmb_log_level_char[LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL] = 'F', [TMB_LEVEL_ERROR] = 'E', [TMB_LEVEL_WARNING] = 'W',
    [TMB_LEVEL_INFO] = 'I',  [TMB_LEVEL_DEBUG] = 'D', [TMB_LEVEL_TRACE] = 'T',
};

static tmb_logger_t default_logger     = { 0 };
static bool default_logger_initialized = false;

static inline void fill_ctx(tmb_log_ctx_t* ctx,
                            tmb_string_builder_t user_message,
                            tmb_time_stamp_t ts) {
    ctx->message     = user_message.items;
    ctx->message_len = user_message.length;
    ctx->ts_nsec     = ts.nsec;
    ctx->ts_sec      = ts.sec;
}

void tmb_tee_logger_add_logger(tmb_tee_logger_t* tee_logger,
                               tmb_logger_t* lgr) {
    da_append(tee_logger, lgr);
}

void tmb_print_version(void) {
    printf("%s.%s.%s @ %s\nSO Version: %s\n",
           TMB_MAJOR_V,
           TMB_MINOR_V,
           TMB_PATCH_V,
           GIT_REV,
           TMB_SO_V);
}

const char* tmb_get_version(void) {
    static tmb_string_builder_t sb = { 0 };
    if (sb.length == 0) {
        sb_appendf__(&sb,
                     "%s.%s.%s @ %s\nSO Version: %s\n",
                     TMB_MAJOR_V,
                     TMB_MINOR_V,
                     TMB_PATCH_V,
                     GIT_REV,
                     TMB_SO_V);
        sb_to_cstr(&sb);
    }
    return sb.items;
}

int tmb_logger_add_sink(tmb_logger_t* logger, tmb_sink_t sink) {
    da_append(&logger->sinks, sink);
    da_append(&logger->sink_formatter_map, 0);
    return logger->sinks.length - 1;
}

int tmb_logger_add_formatter(tmb_logger_t* lgr, tmb_formatter_t formatter) {
    da_append(&lgr->formatters, formatter);
    return lgr->formatters.length - 1;
}

int tmb_logger_add_format(tmb_logger_t* lgr, const char* fmt) {
    tmb_formatter_t formatter = { 0 };
    if (tmb_formatter_init(&formatter, fmt)) {
        da_append(&lgr->formatters, formatter);
        return lgr->formatters.length - 1;
    } else {
        da_free(&formatter);
        return -1;
    }
}

int tmb_logger_assign_format(tmb_logger_t* lgr, int sink_idx, int fmt_idx) {
    if (sink_idx >= lgr->sinks.length ||
        sink_idx >= lgr->sink_formatter_map.length) {
        return -1;
    }
    if (fmt_idx >= lgr->formatters.length) { return -2; }
    lgr->sink_formatter_map.items[sink_idx] = fmt_idx;
    return 1;
}

int tmb_logger_set_format(tmb_logger_t* lgr, int sink_idx, const char* fmt) {
    if (sink_idx >= lgr->sinks.length ||
        sink_idx >= lgr->sink_formatter_map.length) {
        return -1;
    }
    tmb_formatter_t formatter = { 0 };
    if (!tmb_formatter_init(&formatter, fmt)) { return -2; }
    da_append(&lgr->formatters, formatter);
    lgr->sink_formatter_map.items[sink_idx] = lgr->formatters.length - 1;
    return -1;
}

bool tmb_logger_set_default_format(tmb_logger_t* logger, const char* fmt) {
    if (logger->formatters.length == 0) {
        da_append(&logger->formatters, (tmb_formatter_t) { 0 });
    } else {
        da_free(&logger->formatters.items[0]);
    }
    return tmb_formatter_init(&logger->formatters.items[0], fmt);
}

tmb_logger_t* tmb_get_default_logger() {
    if (!default_logger_initialized) {
        tmb_logger_set_default_format(&default_logger, TMB_DEFAULT_FORMAT);
        tmb_logger_add_sink(&default_logger, TMB_SINK_STDERR());
        default_logger.max_log_level = LOG_LEVEL_INFO;
        default_logger_initialized   = true;
    }
    return &default_logger;
}

tmb_logger_t* tmb_logger_create(const char* logger_name) {
    tmb_logger_t* lgr = malloc(sizeof(*lgr));
    memset(lgr, 0, sizeof(*lgr));

    lgr->max_log_level  = LOG_LEVEL_INFO;
    int logger_name_len = (int)strlen(logger_name);
    int len_to_cpy      = 0;
    if (logger_name_len > MAX_LOGGER_NAME_LEN) {
        len_to_cpy = MAX_LOGGER_NAME_LEN;
    } else {
        len_to_cpy = logger_name_len;
    }

    for (int i = 0; i < len_to_cpy; i++) { lgr->name[i] = logger_name[i]; }

    return lgr;
}

TMB_API void tmb_logger_destroy(tmb_logger_t* logger) {
    // todo cleanup this destroy mess
    for (int i = 0; i < logger->sinks.length; i++) {
        tmb_sink_t* sink = &logger->sinks.items[i];
        sink->free_fn(sink->sink_data);
    }

    for (int i = 0; i < logger->formatters.length; i++) {
        tmb_formatter_t* fmt = &logger->formatters.items[i];
        fmt->data_free_fn(fmt->data);
        for (int j = 0; j < fmt->length; j++) {
            tmb_chip_t* chip = &fmt->items[j];
            if (chip->type == CHIP_TYPE_CONST_VAL) {
                free((void*)chip->const_val.items);
            }
        }
        da_free(fmt);
    }

    da_free(&logger->formatters);
    da_free(&logger->sinks);
    da_free(&logger->sink_formatter_map);
    free(logger);
    logger = NULL;
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

TMB_API void tmb_set_options(tmb_cfg_t cfg) {
    tmb_cfg = cfg;
}

/// LOGGING FUNCTIONS
static inline void tmb_log_impl_ext_ctx__(tmb_log_ctx_t ctx,
                                          tmb_logger_t* logger) {
    if (ctx.log_level > logger->max_log_level) { return; }
    if (ctx.log_level > logger->max_log_level) { return; }

    if (logger->formatters.length < 1) {
        tmb_logger_set_default_format(logger, TMB_DEFAULT_FORMAT);
    }
    struct {
        _da_header_(tmb_formatted_msg_t*);
    } formatted_messages = { 0 };

    for (int j = 0; j < logger->formatters.length; j++) {
        tmb_formatter_t* formatter = &logger->formatters.items[j];
        da_append(&formatted_messages,
                  formatter->format_fn(
                          formatter,
                          &ctx,
                          (tmb_format_opt_t) {
                                  .use_color = tmb_cfg.enable_colors }));
    }

    for (int i = 0; i < logger->sinks.length; i++) {
        int fmt_idx = logger->sink_formatter_map.items[i];
        logger->sinks.items[i].sink_fn(formatted_messages.items[fmt_idx].items,
                                       formatted_messages.items[fmt_idx].length,
                                       logger->sinks.items[i].sink_data);
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
    tmb_time_stamp_t ts = tmb_time_stamp();

    tmb_string_builder_t message_filled = { 0 };
    sb_appendv(&message_filled, message, args);

    fill_ctx(&ctx, message_filled, ts);

    tmb_log_impl_ext_ctx__(ctx, logger);
    sb_free(&message_filled);
}

void tmb_tee_log(tmb_log_ctx_t ctx,
                 const tmb_tee_logger_t* tee_logger,
                 const char* message,
                 ...) {
    tmb_time_stamp_t ts = tmb_time_stamp();

    tmb_string_builder_t message_filled = { 0 };

    va_list args;
    va_start(args, message);
    sb_appendv(&message_filled, message, args);
    va_end(args);

    fill_ctx(&ctx, message_filled, ts);

    for (int i = 0; i < tee_logger->length; i++) {
        tmb_log_impl_ext_ctx__(ctx, tee_logger->items[i]);
    }
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
