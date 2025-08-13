#define TMB_MIN_LOG_LEVEL 7
#include <tmb.h>

int main(void) {
    tmb_logger_t logger1 = { 0 };
    tmb_logger_t logger2 = { 0 };
    tmb_logger_set_format(&logger1, "log1 [%L] %m\n");
    tmb_logger_set_format(&logger2, "log2 {[%L] %m}\n");

    tmb_tee_logger_t tee_logger = { 0 };
    tmb_tee_logger_add_logger(&tee_logger, &logger1);
    tmb_tee_logger_add_logger(&tee_logger, &logger2);

    LOG_FATAL(&tee_logger, "message %d", 3);
    LOG_ERROR(&tee_logger, "message %d", 3);
    LOG_WARNING(&tee_logger, "message %d", 3);
    LOG_INFO(&tee_logger, "message %d", 3);
    LOG_DEBUG(&tee_logger, "message %d", 3);
    LOG_TRACE(&tee_logger, "message %d", 3);
}
