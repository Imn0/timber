#include <tmb/tmb.h>

int main(void) {
    tmb_logger_t* logger = TMB_LOGGER("my logger",
                                      .log_level = TMB_LOG_LEVEL_DEBUG);

    TMB_LOGGER_ADD(
            logger,
            TMB_SINK_STDOUT(),
            TMB_FORMAT("{D} {d} {$BLACK:$DIM}{@}:{#}{$RESET} {$LEVEL:$}\n"));

    TMB_LOGGER_ADD(logger,
                   TMB_SINK_STDERR(.min_log_level = TMB_LOG_LEVEL_WARNING),
                   TMB_FORMAT_JSON(.has_timestamp = true));

    TMB_LOGGER_ADD(
            logger,
            TMB_SINK_ROTATING_FILE(
                    "log.txt", 5, 1024, .min_log_level = TMB_LOG_LEVEL_ERROR),
            TMB_FORMAT("{D} {d} {@}:{#} {$}\n"));

    LOG_FATAL(logger, "just a message");
    LOG_ERROR(logger, "a message with number %d", 3);
    LOG_WARNING(logger, "literally just %s", "printf"); // no file error here

    LOG_INFO(logger, "message %d", 3);        // no json logs here
    LOG_DEBUG(logger, "fancy message %d", 3); // no json logs here

    tmb_logger_destroy(logger);
}
