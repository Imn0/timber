#include <tmb/tmb.h>

int main(void) {
    // just use it
    LOG_FATAL("just a message");
    LOG_ERROR("a message with number %d", 3);
    LOG_WARNING("literrally just %s", "printf");
    LOG_INFO("message %d", 3);
    LOG_DEBUG("debug wont show by default");

    tmb_get_default_logger()->cfg.max_log_level = LOG_LEVEL_DEBUG;
    LOG_DEBUG("and now it will");

    TMB_CFG(.enable_colors = false, .max_log_level = LOG_LEVEL_WARNING);
    LOG_FATAL("wow");
    LOG_INFO("and this wont show");
}
