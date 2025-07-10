#include <tmb.h>

static Logger default_logger;

void tmb_init(void) {
    tmb_logger_init_default(&default_logger);
}

void tmb_deinit(void) {
    tmb_logger_deinit(&default_logger);
}

const Logger* tmb_get_default_logger(void) {
    return &default_logger;
}
