#include <chips.h>
#include <sink.h>
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
    [TMB_LEVEL_FATAL] = "FATAL",     [TMB_LEVEL_ERROR] = "ERROR",
    [TMB_LEVEL_WARNING] = "WARNING", [TMB_LEVEL_INFO] = "INFO",
    [TMB_LEVEL_DEBUG] = "DEBUG",     [TMB_LEVEL_TRACE] = "TRACE",
};

const char* const tmb_log_level_color[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = MAKE_ANSI(ANSI_RED, ANSI_BOLD),
    [TMB_LEVEL_ERROR]   = MAKE_ANSI(ANSI_RED),
    [TMB_LEVEL_WARNING] = MAKE_ANSI(ANSI_YELLOW),
    [TMB_LEVEL_INFO]    = MAKE_ANSI(ANSI_GREEN),
    [TMB_LEVEL_DEBUG]   = MAKE_ANSI(ANSI_BLUE),
    [TMB_LEVEL_TRACE]   = MAKE_ANSI(ANSI_BLACK, ANSI_ITALIC),
};

const int tmb_log_level_str_len[TMB_LOG_LEVEL_COUNT] = {
    [TMB_LEVEL_FATAL]   = TMB_CONST_STR_SIZE("FATAL"),
    [TMB_LEVEL_ERROR]   = TMB_CONST_STR_SIZE("ERROR"),
    [TMB_LEVEL_WARNING] = TMB_CONST_STR_SIZE("WARNING"),
    [TMB_LEVEL_INFO]    = TMB_CONST_STR_SIZE("INFO"),
    [TMB_LEVEL_DEBUG]   = TMB_CONST_STR_SIZE("DEBUG"),
    [TMB_LEVEL_TRACE]   = TMB_CONST_STR_SIZE("TRACE"),
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
        .log_level    = ctx.log_level,
        .line_no      = ctx.line_no,
        .filename     = ctx.filename,
        .filename_len = ctx.filename_len,
        .funcname     = ctx.funcname,
        .funcname_len = ctx.funcname_len,
        .message      = user_message.items,
        .message_len  = user_message.size,
        .ts           = tmb_timestamp(),
    };
}

static inline void tmb_log_impl_ext_ctx__(tmb_log_ext_ctx_t ext_ctx,
                                          const tmb_logger_t* logger) {
    tmb_string_builder_t log_message = { 0 };

    for (int i = 0; i < logger->chips.size; i++) {
        tmb_chip_t* current_chip = &logger->chips.items[i];
        if (current_chip->just_amount > 0) {
            tmb_string_builder_t sb = { 0 };
            current_chip->chip_fn(&sb, &ext_ctx, current_chip->chip_data);
            if (sb.size < current_chip->just_amount) {
                tmb_sb_just(&sb,
                            current_chip->just_opt,
                            current_chip->just_amount,
                            ' ');
            } else if (sb.size > current_chip->just_amount) {
                tmb_sb_truncate(&sb,
                                current_chip->truncate_opt,
                                current_chip->just_amount);
            }
            sb_appendn(&log_message, sb.items, sb.size);
            sb_free(&sb);
        } else {
            current_chip->chip_fn(&log_message,
                                  &ext_ctx,
                                  current_chip->chip_data);
        }
    }
    sb_to_cstr(&log_message);
    for (int i = 0; i < logger->sinks.size; i++) {
        logger->sinks.items[i].sink_fn(log_message.items,
                                       log_message.size,
                                       logger->sinks.items[i].sink_data);
    }
    sb_free(&log_message);
}

static inline void tmb_log_impl__(tmb_log_ctx_t ctx,
                                  const tmb_logger_t* logger,
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
             const tmb_logger_t* logger,
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
    if (sb.size == 0) {
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

static inline tmb_chip_t chip_make(char fmt_opt,
                                   char truncate_opt_chr,
                                   char just_opt_chr,
                                   int just_amount) {
    tmb_chip_t chip = { 0 };
    switch (fmt_opt) {
    case 'm':
        chip = TMB_CHIP_MESSAGE();
        break;
    case 'l':
        chip = TMB_CHIP_LEVEL_L();
        break;
    case 'L':
        chip = TMB_CHIP_LEVEL_S();
        break;
    default:
        fprintf(stderr, "unknown format %c\n", fmt_opt);
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
    chip.just_amount  = just_amount;
    chip.truncate_opt = truncate_opt;
    chip.just_opt     = just_opt;

    return chip;
}

static inline void logger_push_chip(tmb_logger_t* logger, tmb_chip_t chip) {
    da_append(&logger->chips, chip);
}

void tmb_logger_add_sink(tmb_logger_t* logger, tmb_sink_t sink) {
    da_append(&logger->sinks, sink);
}

bool tmb_logger_set_format(tmb_logger_t* logger, const char* fmt) {
    UNUSED logger;
    tmb_string_builder_t sb = { 0 };
    int fmt_len             = (int)strlen(fmt);

    for (int i = 0; i < fmt_len; i++) {
        if (fmt[i] == '%') {
            if (sb.size > 0) {
                logger_push_chip(logger, TMB_CHIP_CONST(sb.items, sb.size));
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
            logger_push_chip(
                    logger,
                    chip_make(fmt[i], truncate_opt, just_opt, just_amount));
        } else {
            sb_append(&sb, fmt[i]);
        }
    }
    if (sb.size > 0) {
        logger_push_chip(logger, TMB_CHIP_CONST(sb.items, sb.size));
        sb_free(&sb);
    }

    return true;
}

tmb_logger_t* tmb_get_default_logger() {
    if (!default_logger_initialized) {
        tmb_logger_set_format(&default_logger, "[%7^l] %m\n");
        tmb_logger_add_sink(&default_logger, TMB_SINK_STDERR());
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

    for (int i = 0; i < tee_logger->size; i++) {
        tmb_log_impl_ext_ctx__(e_ctx, tee_logger->items[i]);
    }
}
