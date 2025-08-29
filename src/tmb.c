#include <chips.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

const char* const TMB_PATCH_V = "0";
const char* const TMB_MINOR_V = "0";
const char* const TMB_MAJOR_V = "0";
const char* const TMB_SO_V    = "0";

const char* const tmb_log_level_str[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_LEVEL_FATAL_STR,
    [TMB_LEVEL_ERROR]   = TMB_LEVEL_ERROR_STR,
    [TMB_LEVEL_WARNING] = TMB_LEVEL_WARNING_STR,
    [TMB_LEVEL_INFO]    = TMB_LEVEL_INFO_STR,
    [TMB_LEVEL_DEBUG]   = TMB_LEVEL_DEBUG_STR,
    [TMB_LEVEL_TRACE]   = TMB_LEVEL_TRACE_STR,
};

const char* const tmb_log_level_color[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_LEVEL_FATAL_COLOR,
    [TMB_LEVEL_ERROR]   = TMB_LEVEL_ERROR_COLOR,
    [TMB_LEVEL_WARNING] = TMB_LEVEL_WARNING_COLOR,
    [TMB_LEVEL_INFO]    = TMB_LEVEL_INFO_COLOR,
    [TMB_LEVEL_DEBUG]   = TMB_LEVEL_DEBUG_COLOR,
    [TMB_LEVEL_TRACE]   = TMB_LEVEL_TRACE_COLOR,
};

const int tmb_log_level_str_len[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_CONST_STR_SIZE(TMB_LEVEL_FATAL_STR),
    [TMB_LEVEL_ERROR]   = TMB_CONST_STR_SIZE(TMB_LEVEL_ERROR_STR),
    [TMB_LEVEL_WARNING] = TMB_CONST_STR_SIZE(TMB_LEVEL_WARNING_STR),
    [TMB_LEVEL_INFO]    = TMB_CONST_STR_SIZE(TMB_LEVEL_INFO_STR),
    [TMB_LEVEL_DEBUG]   = TMB_CONST_STR_SIZE(TMB_LEVEL_DEBUG_STR),
    [TMB_LEVEL_TRACE]   = TMB_CONST_STR_SIZE(TMB_LEVEL_TRACE_STR),
};

const char tmb_log_level_char[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL] = 'F', [TMB_LEVEL_ERROR] = 'E', [TMB_LEVEL_WARNING] = 'W',
    [TMB_LEVEL_INFO] = 'I',  [TMB_LEVEL_DEBUG] = 'D', [TMB_LEVEL_TRACE] = 'T',
};

static inline tmb_time_stamp_t tmb_timestamp() {
    // extern inline tmb_time_stamp_t tmb_timestamp();
    long long sec  = 0;
    long long nsec = 0;

#if defined(TMB_UNIX)
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_REALTIME, &ts);
    sec  = ts.tv_sec;
    nsec = ts.tv_nsec;
#elif defined(TMB_WINDOWS)
    sec = time(NULL);
#else
    sec = time(NULL);
#endif

    return (tmb_time_stamp_t) { .sec = sec, .nsec = nsec };
}

static tmb_logger_t default_logger     = { 0 };
static bool default_logger_initialized = false;

static inline tmb_log_ext_ctx_t ext_ctx_from_ctx(
        tmb_log_ctx_t ctx,
        tmb_string_builder_t user_message) {

    return (tmb_log_ext_ctx_t) {
        .log_level         = ctx.log_level,
        .line_no           = ctx.line_no,
        .filename          = ctx.filename,
        .filename_len      = ctx.filename_len,
        .filename_base     = ctx.filename_base,
        .filename_base_len = ctx.filename_base_len,
        .funcname          = ctx.funcname,
        .funcname_len      = ctx.funcname_len,
        .message           = user_message.items,
        .message_len       = user_message.length,
        .ts                = tmb_timestamp(),
    };
}

static inline void tmb_log_impl_ext_ctx__(tmb_log_ext_ctx_t ext_ctx,
                                          tmb_logger_t* logger) {
    if (logger->formatters.length < 1) {
        tmb_logger_set_default_format(logger, "%v");
    }
    struct {
        _da_header_(tmb_string_builder_t*);
    } formatted_messages = { 0 };

    for (int j = 0; j < logger->formatters.length; j++) {
        tmb_formater_t* chips = &logger->formatters.items[j];
        da_append(&formatted_messages, (tmb_string_builder_t) { 0 });

        for (int i = 0; i < chips->length; i++) {
            tmb_chip_t* current_chip = &chips->items[i];
            if (current_chip->just_amount > 0) {
                tmb_string_builder_t sb = { 0 };
                current_chip->chip_fn(&sb, &ext_ctx, current_chip->chip_data);
                if (sb.length < current_chip->just_amount) {
                    tmb_sb_just(&sb,
                                current_chip->just_opt,
                                current_chip->just_amount,
                                ' ');
                } else if (sb.length > current_chip->just_amount) {
                    tmb_sb_truncate(&sb,
                                    current_chip->truncate_opt,
                                    current_chip->just_amount);
                }
                sb_appendn(&formatted_messages.items[j], sb.items, sb.length);
                sb_free(&sb);
            } else {
                current_chip->chip_fn(&formatted_messages.items[j],
                                      &ext_ctx,
                                      current_chip->chip_data);
            }
        }
        sb_to_cstr(&formatted_messages.items[j]);
    }

    for (int i = 0; i < logger->sinks.length; i++) {
        int fmt_idx = logger->sink_formatter_map.items[i];
        logger->sinks.items[i].sink_fn(formatted_messages.items[fmt_idx].items,
                                       formatted_messages.items[fmt_idx].length,
                                       logger->sinks.items[i].sink_data);
    }
    da_free_memb_fn(&formatted_messages, items, free);
}

static inline void tmb_log_impl__(tmb_log_ctx_t ctx,
                                  tmb_logger_t* logger,
                                  const char* message,
                                  va_list args) {

    tmb_string_builder_t message_filled = { 0 };
    sb_appendv(&message_filled, message, args);

    tmb_log_ext_ctx_t e_ctx = ext_ctx_from_ctx(ctx, message_filled);

    tmb_log_impl_ext_ctx__(e_ctx, logger);
    sb_free(&message_filled);
}

void tmb_tee_logger_add_logger(tmb_tee_logger_t* tee_logger,
                               tmb_logger_t* lgr) {
    da_append(tee_logger, lgr);
}

void tmb_log(tmb_log_ctx_t ctx,
             tmb_logger_t* logger,
             const char* message,
             ...) {
    if (ctx.log_level > logger->log_level) { return; }
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

static inline bool chip_init(tmb_chip_t* chip,
                             char fmt_opt,
                             char truncate_opt_chr,
                             char just_opt_chr,
                             int just_amount) {
#define CASE(c, fn)                                                            \
    case c:                                                                    \
        *chip = fn();                                                          \
        break

    switch (fmt_opt) {
        CASE('$', TMB_CHIP_MESSAGE);
        CASE('l', TMB_CHIP_LEVEL_L);
        CASE('L', TMB_CHIP_LEVEL_S);
        CASE('@', TMB_CHIP_BASEFILE);
        CASE('s', TMB_CHIP_FILE);
        CASE('#', TMB_CHIP_LINE);
        CASE('f', TMB_CHIP_FUNC);
    default:
        fprintf(stderr, "unknown format %c\n", fmt_opt);
        return false;
        break;
    }
    enum tmb_sb_truncate_opt truncate_opt = TRUNCATE_OFF;
    enum tmb_sb_just_opt just_opt         = JUST_OFF;
    switch (truncate_opt_chr) {
    case TRUNCATING_LEFT_OPT_CHAR:
        truncate_opt = TRUNCATE_LEFT;
        break;
    case TRUNCATING_RIGHT_OPT_CHAR:
        truncate_opt = TRUNCATE_RIGHT;
        break;
    default:
        break;
    }

    switch (just_opt_chr) {
    case JUSTING_LEFT_OPT_CHAR:
        just_opt = JUST_LEFT;
        break;
    case JUSTING_CENTER_OPT_CHAR:
        just_opt = JUST_CENTER;
        break;
    case JUSTING_RIGHT_OPT_CHAR:
        just_opt = JUST_RIGHT;
        break;
    default:
        break;
    }
    chip->just_amount  = just_amount;
    chip->truncate_opt = truncate_opt;
    chip->just_opt     = just_opt;
    return true;
}

int tmb_logger_add_sink(tmb_logger_t* logger, tmb_sink_t sink) {
    da_append(&logger->sinks, sink);
    da_append(&logger->sink_formatter_map, 0);
    return logger->sinks.length - 1;
}

bool tmb_format_init(tmb_formater_t* format, const char* fmt) {
    tmb_string_builder_t sb = { 0 };
    int fmt_len             = (int)strlen(fmt);

    for (int i = 0; i < fmt_len; i++) {
        if (fmt[i] == '%') {
            if (sb.length > 0) {
                da_append(format, TMB_CHIP_CONST(sb.items, sb.length));
                sb_free(&sb);
            }
            i++;
            char just_opt     = 0;
            char truncate_opt = 0;
            int just_amount   = 0;
            if (isdigit(fmt[i])) {
                while (isdigit(fmt[i])) {
                    just_amount *= 10;
                    just_amount += (fmt[i] - '0');
                    i++;
                }
            }
            if (fmt[i] == JUSTING_LEFT_OPT_CHAR ||
                fmt[i] == JUSTING_CENTER_OPT_CHAR ||
                fmt[i] == JUSTING_RIGHT_OPT_CHAR) {
                just_opt = fmt[i++];
            }
            if (fmt[i] == TRUNCATING_LEFT_OPT_CHAR ||
                fmt[i] == TRUNCATING_RIGHT_OPT_CHAR) {
                truncate_opt = fmt[i++];
            }
            da_append(format, (tmb_chip_t) { 0 });
            if (!chip_init(&format->items[format->length - 1],
                           fmt[i],
                           truncate_opt,
                           just_opt,
                           just_amount)) {
                return false;
            }
        } else {
            sb_append(&sb, fmt[i]);
        }
    }
    if (sb.length > 0) {
        da_append(format, TMB_CHIP_CONST(sb.items, sb.length));
        sb_free(&sb);
    }
    return true;
}

int tmb_logger_add_formater(tmb_logger_t* lgr, tmb_formater_t formater) {
    da_append(&lgr->formatters, formater);
    return lgr->formatters.length - 1;
}

int tmb_logger_add_format(tmb_logger_t* lgr, const char* fmt) {
    tmb_formater_t format_chips = { 0 };
    if (tmb_format_init(&format_chips, fmt)) {
        da_append(&lgr->formatters, format_chips);
        return lgr->formatters.length - 1;
    } else {
        da_free(&format_chips);
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
    tmb_formater_t format = { 0 };
    if (!tmb_format_init(&format, fmt)) { return -2; }
    da_append(&lgr->formatters, format);
    lgr->sink_formatter_map.items[sink_idx] = lgr->formatters.length - 1;
    return -1;
}

bool tmb_logger_set_default_format(tmb_logger_t* logger, const char* fmt) {
    if (logger->formatters.length == 0) {
        da_append(&logger->formatters, (tmb_formater_t) { 0 });
    } else {
        da_free(&logger->formatters.items[0]);
    }
    return tmb_format_init(&logger->formatters.items[0], fmt);
}

tmb_logger_t* tmb_get_default_logger() {
    if (!default_logger_initialized) {
        tmb_logger_set_default_format(&default_logger, TMB_DEFAULT_FORMAT);
        tmb_logger_add_sink(&default_logger, TMB_SINK_STDERR());
        default_logger.log_level   = TMB_LOG_LEVEL_INFO;
        default_logger_initialized = true;
    }
    return &default_logger;
}

void tmb_tee_log(tmb_log_ctx_t ctx,
                 const tmb_tee_logger_t* tee_logger,
                 const char* message,
                 ...) {

    tmb_string_builder_t message_filled = { 0 };

    va_list args;
    va_start(args, message);
    sb_appendv(&message_filled, message, args);
    va_end(args);

    tmb_log_ext_ctx_t e_ctx = ext_ctx_from_ctx(ctx, message_filled);

    for (int i = 0; i < tee_logger->length; i++) {
        tmb_log_impl_ext_ctx__(e_ctx, tee_logger->items[i]);
    }
}
