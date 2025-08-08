#include <stdarg.h>

#include <format.h>
#include <sink.h>
#include <tmb.h>

struct sinks_t {
    _da_header_(Sink**);
};

struct formatters_t {
    _da_header_(Formatter**);
};

struct fmt_map_t {
    _da_header_(size_t*);
};

void tmb_log_impl(LogCtx ctx,
                  const Logger* logger,
                  const char* message,
                  va_list args);

String fmt_format(Formatter* fmt,
                  const LogCtx* ctx,
                  const cstr msg_fmt,
                  va_list msg_arg);

size_t tmb_logger_add_formatter(Logger* lg, Formatter fmt);
size_t tmb_logger_add_sink(Logger* lg, Sink sink);
bool tmb_logger_fmt_sink_assign(Logger* lg,
                                size_t formmatter_idx,
                                size_t sink_idx);