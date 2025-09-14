#include <tmb/sink.h>
#include <tmb/tmb.h>

int main(void) {
    tmb_logger_t* logger = tmb_logger_create("my logger");
    tmb_logger_add_sink(logger, TMB_SINK_STDERR());
    tmb_logger_set_default_format(logger, "{$}\n");

    LOG_FATAL(logger, "just a message");
    LOG_ERROR(logger, "a message with number %d", 3);
    LOG_WARNING(logger, "literrally just %s", "printf");
    LOG_INFO(logger, "message %d", 3);
    LOG_DEBUG(logger, "message %d", 3);

    tmb_logger_destroy(logger);
}
