#include <stdarg.h>

#include <format.h>
#include <sink.h>
#include <tmb.h>
#include <tmb_internal.h>
#include <tmb_lib.h>

size_t tmb_logger_add_formatter(Logger* lg, Formatter fmt);
size_t tmb_logger_add_sink(Logger* lg, Sink sink);
bool tmb_logger_fmt_sink_assign(Logger* lg,
                                size_t formmatter_idx,
                                size_t sink_idx);

size_t tmb_logger_add_formatter(Logger* lg, Formatter fmt) {
    da_append(&lg->formatters, fmt);

    return lg->formatters.size - 1;
}

size_t tmb_logger_add_sink(Logger* lg, Sink sink) {
    da_append(&lg->sinks, sink);
    da_reserve(&lg->fmt_map, lg->sinks.size);
    return lg->sinks.size - 1;
}

bool tmb_logger_fmt_sink_assign(Logger* lg,
                                size_t formmatter_idx,
                                size_t sink_idx) {
    if (formmatter_idx <= lg->formatters.size || sink_idx <= lg->sinks.size) {
        return false;
    }
    lg->fmt_map.items[sink_idx] = formmatter_idx;
    return true;
}

void tmb_log_impl(LogCtx ctx,
                  const Logger* logger,
                  const char* message,
                  va_list args) {

    struct formated {
        _da_header_(String*);
    } formated = {};
    da_reserve(&formated, logger->formatters.size);

    da_for_each(Formatter, fmt, &logger->formatters) {
        da_append(&formated, fmt_format(fmt, &ctx, message, args));
    }

    for (size_t i = 0; i < logger->sinks.size; i++) {
        Sink* sink = &logger->sinks.items[i];
        sink->sink_log(&formated.items[logger->fmt_map.items[i]],
                       sink->sink_data);
    }

    for (size_t i = 0; i < formated.size; i++) {
        string_free(&formated.items[i]);
    }
    da_free(&formated);
}

bool tmb_logger_init_default(Logger* lg) {
    if (lg == NULL) { return false; }

    Formatter fmt = {};
    da_append(&fmt, tmb_fmt_chip_const_val_make("[ "));
    da_append(&fmt, tmb_fmt_chip_log_level_make(true));
    da_append(&fmt, tmb_fmt_chip_const_val_make(" ] "));
    da_append(&fmt, tmb_fmt_chip_message_make());
    da_append(&fmt, tmb_fmt_chip_const_val_make("\n"));
    tmb_logger_add_formatter(lg, fmt);

    tmb_logger_add_sink(lg, stderror_sink_create());

    return true;
}

bool tmb_logger_deinit(Logger* lg) {
    da_free(&lg->fmt_map);
    for (size_t i = 0; i < lg->formatters.size; i++) {
        da_free_memb_fn(&lg->formatters.items[i], token_data, free_fn);
    }
    da_free(&lg->formatters);
    da_free(&lg->sinks);
    return true;
}
