#include <tmb/tmb.h>

int main(void) {
    tmb_logger_t* logger = TMB_LOGGER("my logger",
                                      .log_level = TMB_LOG_LEVEL_DEBUG);

    tmb_logger_assign_format(
            logger,
            tmb_logger_add_sink(logger, TMB_SINK_STDOUT()),
            tmb_logger_add_formatter(
                    logger,
                    tmb_formatter_make(
                            "{$LEVEL} {D} {d} {$BLACK:$DIM}{@}:{#}{$RESET} {$LEVEL:$}\n")));

    tmb_logger_assign_format(
            logger,
            tmb_logger_add_sink(
                    logger,
                    TMB_SINK_STDERR(.min_log_level = TMB_LOG_LEVEL_WARNING)),
            tmb_logger_add_formatter(logger,
                                     TMB_FORMAT_JSON(.has_timestamp = true)));

    tmb_logger_assign_format(
            logger,
            tmb_logger_add_sink(logger,
                                TMB_SINK_ROTATING_FILE(
                                        "log.txt",
                                        5,
                                        1024,
                                        .min_log_level = TMB_LOG_LEVEL_ERROR)),
            tmb_logger_add_formatter(
                    logger, tmb_formatter_make("{D} {d} {@}:{#} {$}\n")));

    LOG_FATAL(logger, "just a message");
    LOG_ERROR(logger, "a message with number %d", 3);
    LOG_WARNING(logger, "literrally just %s", "printf"); // no file error here

    LOG_INFO(logger, "message %d", 3);        // no json logs here
    LOG_DEBUG(logger, "fancy message %d", 3); // no json logs here

    tmb_logger_destroy(logger);
}
