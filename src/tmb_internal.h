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