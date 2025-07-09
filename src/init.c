#include <tmb.h>

static Logger default_logger;

void tmb_init() {
    tmb_logger_init_default(&default_logger);
}

void tmb_deinit() {
    tmb_logger_destroy(&default_logger);
}

const Logger* tmb_get_default_logger() {
    return &default_logger;
}
