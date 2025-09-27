#include <tmb/sink.h>
#include <tmb/tmb.h>

int main(void) {
    tmb_logger_t* logger = TMB_LOGGER("my logger",
                                      .max_log_level = TMB_LEVEL_DEBUG);
    tmb_logger_set_default_format(logger, "{$}\n");

    LOG_FATAL(logger, "just a message");
    LOG_ERROR(logger, "a message with number %d", 3);
    LOG_WARNING(logger, "literrally just %s", "printf");
    tmb_logger_set_default_format(logger, "{@}:{#} {$BLUE:}{$}{$RESET}\n");
    LOG_INFO(logger, "message %d", 3);
    LOG_DEBUG(logger, "fancy message %d", 3);

    tmb_logger_destroy(logger);
}
