#include <tmb/tmb.h>

int main(void) {
    tmb_sink_t* stderr_sink = TMB_SINK_STDERR();
    tmb_logger_t* logger1   = TMB_LOGGER("my logger 1");
    tmb_logger_add_sink(logger1, stderr_sink);
    tmb_logger_set_default_format(logger1, "{n}: {$MAGENTA:T} {$}\n");
    tmb_logger_add_tag(logger1, "dev");
    tmb_register_logger("logger1", logger1);

    tmb_logger_t* logger2 = TMB_LOGGER("my logger 2");
    tmb_logger_add_sink(logger2, stderr_sink);
    tmb_logger_set_default_format(logger2, "{n}: {$MAGENTA:T} {$}\n");
    tmb_logger_add_tag(logger2, "prod");
    tmb_register_logger("logger2", logger2);

    tmb_logger_t* lgr = tmb_get_logger("logger1");
    LOG_INFO(lgr, "a message");

    lgr = tmb_get_logger("logger2");
    LOG_INFO(lgr, "a message");

    lgr = tmb_get_logger_or_default("this one doesnt exist");
    LOG_INFO(lgr, "a message");

    tmb_logger_destroy(logger1);
    tmb_logger_destroy(logger2);
}
