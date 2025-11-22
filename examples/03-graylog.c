#include <tmb/tmb.h>

// https://graylog.org/
int main(void) {
#ifndef _MSC_VER
    tmb_logger_t* logger = TMB_LOGGER("my logger");

    tmb_logger_assign_format(
            logger,
            tmb_logger_add_sink(
                    logger,
                    TMB_SINK_GRAYLOG("127.0.0.1", 12201)),
            tmb_logger_add_formatter(logger, TMB_FORMAT_GRAYLOG()));

    LOG_FATAL(logger, "just a message");
    LOG_ERROR(logger, "a message with number %d", 3);
    LOG_WARNING(logger, "literally just %s", "printf");

    tmb_logger_destroy(logger);
#endif
}
