#include <tmb/sink.h>
#include <tmb/tmb.h>

int main(void) {

    tmb_logger_t logger = { .min_log_level = LOG_LEVEL_INFO };
    tmb_logger_add_sink(&logger, TMB_SINK_STDERR());
    tmb_logger_set_default_format(&logger, "%$\n");

    LOG_FATAL(&logger, "just a message");
    LOG_ERROR(&logger, "a message with number %d", 3);
    LOG_WARNING(&logger, "literrally just %s", "printf");
    LOG_INFO(&logger, "message %d", 3);
    LOG_DEBUG(&logger, "message %d", 3);
}
